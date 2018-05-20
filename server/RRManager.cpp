#include "RRManager.h"
#include "Helper.h"
#include "Logic/AuthLogic.h"
#include "db/SessionPostgresInfo.h"
#include "db/AuthPostgresManager.h"

RRManager::RRManager(Server& server)
{
    server.subscribe(std::bind(
                         &RRManager::onConnected
                         , this
                         , std::placeholders::_1));
}

void RRManager::onConnected(ClientChannelPtr session)
{
    LOG_INFO("Connected session");
    sessions_.emplace_back(session);

    session->setReadHandle(std::bind(&RRManager::readSessionBuffer
                                     , this
                                     , std::placeholders::_1
                                     , std::placeholders::_2));

    session->setDisconectedHandle(std::bind(&RRManager::disconectedSession
                                            , this
                                            , std::placeholders::_1));
}

void RRManager::readSessionBuffer(std::shared_ptr<ClientChannel> session, ByteBufferPtr buffPtr)
{
    network::RequestContext reqContext;
    network::ResponseContext resContext;
    reqContext.ParseFromString(Helper::bufferToString(buffPtr, 0, 0));

    ResponseCode responseCode;
    switch (reqContext.message_type_()) {
    case network::MO_AUTH:
        responseCode = authRR(reqContext, resContext);
        if(responseCode == ResponseCode::status_success)
        {
            saveSession(session, resContext);
        }
        break;
    case network::MO_REGISTER:
        responseCode = registerRR(reqContext, resContext);
        break;
    default:
        responseCode = ResponseCode::status_unknown_command;
        break;
    }

    resContext.set_error_code(static_cast<int32_t>(responseCode));
    resContext.set_message_type_(reqContext.message_type_());
    session->execute(resContext.SerializeAsString());
}

void RRManager::disconectedSession(std::shared_ptr<ClientChannel> session)
{
    LOG_INFO("Disconected!");
    SessionPostgresInfo::removeSession(session->getSessionId());
}

void RRManager::saveSession(std::shared_ptr<ClientChannel> session, const network::ResponseContext &response)
{
    session->setSessionId(response.session_info().session_id());
    UserInfo userInfo;
    AuthPostgresManager::getUserByLogin(response.session_info().login(), userInfo);
    session->setUserInfo(userInfo);
}

ResponseCode RRManager::authRR(const network::RequestContext &request, network::ResponseContext &response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!request.has_auth_message_())
        {
            LOG_ERR("Where is not register message!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }

        network::AuthMessage authMessage = request.auth_message_();

        if(!authMessage.has_login())
        {
            LOG_ERR("Where is not login value!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }

        if(!authMessage.has_pass())
        {
            LOG_ERR("Where is not password value!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }
        network::AuthMessageResponse* regRes = new network::AuthMessageResponse();
        network::SessionInfo* sessionInfo = new network::SessionInfo();
        resultStatus = AuthLogic::authUser(authMessage, regRes, sessionInfo);

        response.set_allocated_auth_response(regRes);
        response.set_allocated_session_info(sessionInfo);

        LOG_INFO("Send response-message to client " << authMessage.login()
                 << " message [" << regRes->server_message() << "]");
    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::registerRR(const network::RequestContext &requests, network::ResponseContext& response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!requests.has_register_message_())
        {
            LOG_ERR("Where is not register message!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }

        network::RegisterMessage authMessage = requests.register_message_();

        if(!authMessage.has_login())
        {
            LOG_ERR("Where is not login value!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }

        if(!authMessage.has_pass())
        {
            LOG_ERR("Where is not password value!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }
        network::RegisterMessageResponse* regRes = new network::RegisterMessageResponse();

        resultStatus = AuthLogic::createUser(authMessage, regRes);

        response.set_allocated_register_response(regRes);

        LOG_INFO("Send response-message to client " << authMessage.login()
                 << " message [" << regRes->messagetext() << "]");
    }
    while(false);

    return resultStatus;
}
