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
#include <opendaq/mirrored_signal_impl.h>

#include <native_streaming_client_module/common.h>

BEGIN_NAMESPACE_OPENDAQ_NATIVE_STREAMING_CLIENT_MODULE

DECLARE_OPENDAQ_INTERFACE(INativeStreamingSignalPrivate, IBaseObject)
{
    virtual void assignDomainSignal(const SignalPtr& domainSignal) = 0;
    virtual void removeDomainSignal() = 0;
    virtual void assignDescriptor(const DataDescriptorPtr& descriptor) = 0;
};

class NativeStreamingSignalImpl final : public MirroredSignalBase<INativeStreamingSignalPrivate>
{
public:
    explicit NativeStreamingSignalImpl(const ContextPtr& ctx,
                                       const ComponentPtr& parent,
                                       const DataDescriptorPtr& descriptor,
                                       const StringPtr& streamingId);

    // ISignal
    ErrCode INTERFACE_FUNC getDescriptor(IDataDescriptor** descriptor) override;
    ErrCode INTERFACE_FUNC getDomainSignal(ISignal** signal) override;

    StringPtr onGetRemoteId() const override;
    Bool onTriggerEvent(EventPacketPtr eventPacket) override;

    void assignDomainSignal(const SignalPtr& domainSignal) override;
    void removeDomainSignal() override;
    void assignDescriptor(const DataDescriptorPtr& descriptor) override;

private:
    static StringPtr createLocalId(const StringPtr& streamingId);

    StringPtr streamingId;
    DataDescriptorPtr mirroredDataDescriptor;
    SignalPtr mirroredDomainSignal;

    std::mutex signalMutex;
};

END_NAMESPACE_OPENDAQ_NATIVE_STREAMING_CLIENT_MODULE
