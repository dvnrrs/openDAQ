/*
 * Copyright 2022-2023 Blueberry d.o.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <native_streaming_protocol/native_streaming_protocol.h>
#include <native_streaming_protocol/client_session_handler.h>

#include <opendaq/context_ptr.h>
#include <opendaq/logger_ptr.h>
#include <opendaq/logger_component_ptr.h>
#include <opendaq/packet_ptr.h>

#include <native_streaming/client.hpp>

#include <future>

BEGIN_NAMESPACE_OPENDAQ_NATIVE_STREAMING_PROTOCOL

enum class ClientReconnectionStatus
{
    Connected = 0,
    Reconnecting,
    Restored,
    Unrecoverable
};

using OnSignalAvailableCallback = std::function<void(const StringPtr& signalStringId,
                                                     const StringPtr& domainSignalStringId,
                                                     const DataDescriptorPtr& signalDescriptor,
                                                     const StringPtr& name,
                                                     const StringPtr& description)>;
using OnSignalUnavailableCallback = std::function<void(const StringPtr& signalStringId)>;
using OnPacketCallback = std::function<void(const StringPtr& signalStringId, const PacketPtr& packet)>;
using OnSignalSubscriptionAckCallback = std::function<void(const StringPtr& signalStringId, bool subscribed)>;
using OnReconnectionStatusChangedCallback = std::function<void(ClientReconnectionStatus status)>;

class NativeStreamingClientHandler
{
public:
    explicit NativeStreamingClientHandler(const ContextPtr& context,
                                          std::shared_ptr<boost::asio::io_context> ioContextPtr,
                                          OnSignalAvailableCallback signalAvailableHandler,
                                          OnSignalUnavailableCallback signalUnavailableHandler,
                                          OnPacketCallback packetHandler,
                                          OnSignalSubscriptionAckCallback signalSubscriptionAckCallback,
                                          OnReconnectionStatusChangedCallback reconnectionStatusChangedCb);

    ~NativeStreamingClientHandler();

    bool connect(std::string host,
                 std::string port,
                 std::string path = "/");

    void subscribeSignal(const StringPtr& signalStringId);
    void unsubscribeSignal(const StringPtr& signalStringId);
    EventPacketPtr getDataDescriptorChangedEventPacket(const StringPtr& signalStringId);

protected:
    void initClientSessionHandler(SessionPtr session);
    void initClient(std::string host,
                    std::string port,
                    std::string path);

    void handlePacket(const SignalNumericIdType& signalNumericId, const PacketPtr& packet);
    void handleSignal(const SignalNumericIdType& signalNumericId,
                      const StringPtr& signalStringId,
                      const StringPtr& domainSignalStringId,
                      const DataDescriptorPtr& signalDescriptor,
                      const StringPtr& name,
                      const StringPtr& description,
                      bool available);

    void checkReconnectionStatus(const boost::system::error_code& ec);
    void checkProtocolInitializationStatus(const boost::system::error_code& ec);
    void tryReconnect();
    bool isProtocolInitialized(std::chrono::seconds timeout = std::chrono::seconds(0));

    enum class ConnectionResult
    {
        Connected = 0,
        ServerUnreachable,
        ServerUnsupported
    };

    ContextPtr context;
    std::shared_ptr<boost::asio::io_context> ioContextPtr;
    LoggerPtr logger;
    LoggerComponentPtr loggerComponent;
    OnSignalAvailableCallback signalAvailableHandler;
    OnSignalUnavailableCallback signalUnavailableHandler;
    OnPacketCallback packetHandler;
    OnSignalSubscriptionAckCallback signalSubscriptionAckCallback;
    OnReconnectionStatusChangedCallback reconnectionStatusChangedCb;

    std::shared_ptr<boost::asio::steady_timer> reconnectionTimer;
    std::shared_ptr<boost::asio::steady_timer> protocolInitTimer;

    std::shared_ptr<daq::native_streaming::Client> client;
    std::shared_ptr<ClientSessionHandler> sessionHandler;
    std::promise<ConnectionResult> connectedPromise;
    std::future<ConnectionResult> connectedFuture;
    std::promise<void> protocolInitPromise;
    std::future<void> protocolInitFuture;

    std::unordered_map<SignalNumericIdType, StringPtr> signalIds;
};

END_NAMESPACE_OPENDAQ_NATIVE_STREAMING_PROTOCOL
