#ifndef CONNECTIONRESOLVER_H
#define CONNECTIONRESOLVER_H

#include "UopBase.h"
#include "FaceTypeTraits.h"
#include "PublisherConnection.h"
#include "SubscriberConnection.h"
#include "RequesterConnection.h"
#include "ResponderConnection.h"
#include "tinyxml2.h"
#include "FACE/Common.hpp"
#include "FACE/TSS/Common.hpp"
#include <memory>
#include <string>
#include <map>

/// ConnectionResolver
///
/// Bridges the integration layer's runtime configuration to the generated
/// ConnectionTableImpl classes.  For each named connection in an Integration
/// Context it:
///   1. Resolves the Base instance name and TypedTS instance name, either from
///      a parsed uop-connections.xml buffer or from manual setter calls.
///   2. Retrieves both injected pointers from UopBase::GetInjected.
///   3. Calls Base::Create_Connection to obtain a CONNECTION_ID_TYPE.
///   4. Constructs and returns the appropriate XxxConnection object.
///
/// The resolver is constructed by integration-layer code that knows which
/// UopBase instance owns the IC's injections, and is then passed to
/// ConnectionTableImpl::Initialize().
///
/// Configuration: Initialize(configXml, uopName) parses a buffer conforming
/// to uop-connections.xsd, selecting the <connection-table> element whose
/// uop_name attribute matches uopName.  Manual setter methods may be used
/// instead of (or to override) the XML-parsed values.
///
/// Error handling: factory methods return nullptr on any failure (name not
/// resolved, GetInjected throws, Create_Connection error, construction
/// exception).
/// TODO: add logging in the catch/error blocks once a logging facility is
/// available.
class ConnectionResolver {
public:
    /// @param uopBase  The UoP's injection store; must outlive this resolver.
    explicit ConnectionResolver(UopBase* uopBase)
        : m_uopBase(uopBase)
    {}

    // -------------------------------------------------------------------------
    // Configuration
    // -------------------------------------------------------------------------

    /// Initialize from a uop-connections.xml config buffer.
    ///
    /// Parses the buffer conforming to uop-connections.xsd and finds the
    /// <connection-table> element matching uopName.  Populates base and
    /// TypedTS instance names from either the <all-connections> child
    /// (uniform mode) or the <connection_list> child (per-connection mode).
    ///
    /// Manual setter calls made after Initialize() take precedence for any
    /// name they set, allowing per-connection overrides on top of XML config.
    ///
    /// @param configXml  Null-terminated XML text.  nullptr is a no-op.
    /// @param uopName    The uop_name attribute value to match.  nullptr is a
    ///                   no-op.
    void Initialize(const char* configXml, const char* uopName) {
        if (!configXml || !uopName) { return; }

        tinyxml2::XMLDocument doc;
        if (doc.Parse(configXml) != tinyxml2::XML_SUCCESS) {
            // TODO: log parse error: doc.ErrorStr()
            return;
        }

        const tinyxml2::XMLElement* root =
            doc.FirstChildElement("uop-connection-config");
        if (!root) {
            // TODO: log missing root element
            return;
        }

        for (const tinyxml2::XMLElement* table =
                 root->FirstChildElement("connection-table");
             table != nullptr;
             table = table->NextSiblingElement("connection-table"))
        {
            const char* attr = table->Attribute("uop_name");
            if (!attr || std::string(attr) != uopName) { continue; }

            // Uniform mode: <all-connections base="..." typed_ts="..."/>
            const tinyxml2::XMLElement* allConn =
                table->FirstChildElement("all-connections");
            if (allConn) {
                const char* base    = allConn->Attribute("base");
                const char* typedTs = allConn->Attribute("typed_ts");
                if (base)    { m_uniformBaseName    = base; }
                if (typedTs) { m_uniformTypedTsName = typedTs; }
                return;
            }

            // Per-connection mode: <connection_list><connection .../></connection_list>
            const tinyxml2::XMLElement* connList =
                table->FirstChildElement("connection_list");
            if (connList) {
                for (const tinyxml2::XMLElement* conn =
                         connList->FirstChildElement("connection");
                     conn != nullptr;
                     conn = conn->NextSiblingElement("connection"))
                {
                    const char* connName = conn->Attribute("name");
                    const char* base     = conn->Attribute("base");
                    const char* typedTs  = conn->Attribute("typed_ts");
                    if (!connName) { continue; }  // TODO: log malformed entry
                    if (base)    { m_perConnectionBase[connName]  = base; }
                    if (typedTs) { m_perConnectionNames[connName] = typedTs; }
                }
            }
            return;
        }
        // TODO: log no <connection-table uop_name="uopName"> found
    }

    /// Set the Base instance name used for every connection (uniform mode).
    void setUniformBaseName(const std::string& name) {
        m_uniformBaseName = name;
    }

    /// Override the Base instance name for one specific connection.
    void setConnectionBaseName(const std::string& connectionName,
                               const std::string& baseName) {
        m_perConnectionBase[connectionName] = baseName;
    }

    /// Set the TypedTS instance name used for every connection (uniform mode).
    void setUniformTypedTsName(const std::string& name) {
        m_uniformTypedTsName = name;
    }

    /// Override the TypedTS instance name for one specific connection.
    void setConnectionTypedTsName(const std::string& connectionName,
                                  const std::string& typedTsName) {
        m_perConnectionNames[connectionName] = typedTsName;
    }

    // -------------------------------------------------------------------------
    // Factory methods — one per connection role.
    // Each returns nullptr on failure; exceptions are swallowed internally.
    // -------------------------------------------------------------------------

    template<typename DataType>
    std::unique_ptr<PublisherConnection<DataType>>
    CreatePubConnection(const char* connectionName)
    {
        try {
            using TypedTS = typename Traits<DataType>::TypedTS;
            TypedTS* ts = getTypedTs<TypedTS>(connectionName);
            if (!ts) { return nullptr; }
            FACE::TSS::CONNECTION_ID_TYPE id = createConnection(connectionName);
            if (id < 0) { return nullptr; }
            return std::unique_ptr<PublisherConnection<DataType>>(
                new PublisherConnection<DataType>(ts, id));
        } catch (...) {
            // TODO: log failure for connectionName
            return nullptr;
        }
    }

    template<typename DataType>
    std::unique_ptr<SubscriberConnection<DataType>>
    CreateSubConnection(const char* connectionName)
    {
        try {
            using TypedTS = typename Traits<DataType>::TypedTS;
            TypedTS* ts = getTypedTs<TypedTS>(connectionName);
            if (!ts) { return nullptr; }
            FACE::TSS::CONNECTION_ID_TYPE id = createConnection(connectionName);
            if (id < 0) { return nullptr; }
            return std::unique_ptr<SubscriberConnection<DataType>>(
                new SubscriberConnection<DataType>(ts, id));
        } catch (...) {
            // TODO: log failure for connectionName
            return nullptr;
        }
    }

    template<typename RequestType, typename ResponseType>
    std::unique_ptr<RequesterConnection<RequestType, ResponseType>>
    CreateReqConnection(const char* connectionName)
    {
        try {
            using TypedTS = typename Traits<RequestType>::TypedTS;
            TypedTS* ts = getTypedTs<TypedTS>(connectionName);
            if (!ts) { return nullptr; }
            FACE::TSS::CONNECTION_ID_TYPE id = createConnection(connectionName);
            if (id < 0) { return nullptr; }
            return std::unique_ptr<RequesterConnection<RequestType, ResponseType>>(
                new RequesterConnection<RequestType, ResponseType>(ts, id));
        } catch (...) {
            // TODO: log failure for connectionName
            return nullptr;
        }
    }

    template<typename RequestType, typename ResponseType, typename ISender>
    std::unique_ptr<ResponderConnection<RequestType, ResponseType, ISender>>
    CreateRspConnection(const char* connectionName)
    {
        try {
            using TypedTS = typename Traits<RequestType>::TypedTS;
            TypedTS* ts = getTypedTs<TypedTS>(connectionName);
            if (!ts) { return nullptr; }
            FACE::TSS::CONNECTION_ID_TYPE id = createConnection(connectionName);
            if (id < 0) { return nullptr; }
            return std::unique_ptr<ResponderConnection<RequestType, ResponseType, ISender>>(
                new ResponderConnection<RequestType, ResponseType, ISender>(ts, id));
        } catch (...) {
            // TODO: log failure for connectionName
            return nullptr;
        }
    }

private:
    // -------------------------------------------------------------------------
    // Name resolution helpers
    // -------------------------------------------------------------------------

    /// Return the Base instance name for the given connection.
    /// Per-connection override takes precedence over the uniform name.
    const std::string& resolveBaseName(const char* connectionName) const {
        auto it = m_perConnectionBase.find(connectionName);
        if (it != m_perConnectionBase.end()) { return it->second; }
        return m_uniformBaseName;
    }

    /// Return the TypedTS instance name for the given connection.
    /// Per-connection override takes precedence over the uniform name.
    const std::string& resolveTypedTsName(const char* connectionName) const {
        auto it = m_perConnectionNames.find(connectionName);
        if (it != m_perConnectionNames.end()) { return it->second; }
        return m_uniformTypedTsName;
    }

    // -------------------------------------------------------------------------
    // Injection-store accessors
    // -------------------------------------------------------------------------

    /// Retrieve the injected FACE::TSS::Base* for the given connection.
    /// Returns nullptr if the name is empty or the pointer is not injected.
    FACE::TSS::Base* getBase(const char* connectionName) {
        const std::string& baseName = resolveBaseName(connectionName);
        if (baseName.empty()) {
            // TODO: log missing Base name for connectionName
            return nullptr;
        }
        try {
            return m_uopBase->GetInjected<FACE::TSS::Base*>(FACE::STRING_TYPE(baseName.c_str()));
        } catch (...) {
            // TODO: log Base lookup failure for baseName / connectionName
            return nullptr;
        }
    }

    /// Retrieve the injected TypedTS* for the given connection.
    /// Returns nullptr if the name is empty or the pointer is not injected.
    template<typename TypedTS>
    TypedTS* getTypedTs(const char* connectionName) {
        const std::string& tsName = resolveTypedTsName(connectionName);
        if (tsName.empty()) {
            // TODO: log missing TypedTS name for connectionName
            return nullptr;
        }
        try {
            return m_uopBase->GetInjected<TypedTS*>(FACE::STRING_TYPE(tsName.c_str()));
        } catch (...) {
            // TODO: log TypedTS lookup failure for tsName / connectionName
            return nullptr;
        }
    }

    // -------------------------------------------------------------------------
    // FACE connection creation
    // -------------------------------------------------------------------------

    /// Resolve the Base* for connectionName and call Create_Connection.
    /// Returns -1 on any failure.
    FACE::TSS::CONNECTION_ID_TYPE createConnection(const char* connectionName) {
        FACE::TSS::Base* base = getBase(connectionName);
        if (!base) { return -1; }

        FACE::TSS::CONNECTION_ID_TYPE id  = -1;
        FACE::TSS::MESSAGE_SIZE_TYPE  mms = 0;
        FACE::RETURN_CODE_TYPE        rc;
        base->Create_Connection(
            FACE::STRING_TYPE(connectionName),
            FACE::INF_TIME_VALUE,
            id,
            mms,
            rc);
        if (rc != FACE::RETURN_CODE_TYPE::NO_ERROR) {
            // TODO: log Create_Connection failure (rc) for connectionName
            return -1;
        }
        return id;
    }

    // -------------------------------------------------------------------------
    // Members
    // -------------------------------------------------------------------------

    UopBase*    m_uopBase;

    std::string m_uniformBaseName;
    std::string m_uniformTypedTsName;

    std::map<std::string, std::string> m_perConnectionBase;
    std::map<std::string, std::string> m_perConnectionNames;
};

#endif // CONNECTIONRESOLVER_H
