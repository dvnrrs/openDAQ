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
#include <opendaq/function_block_type.h>
#include <coreobjects/component_type_impl.h>

BEGIN_NAMESPACE_OPENDAQ

class FunctionBlockTypeImpl : public GenericComponentTypeImpl<IFunctionBlockType>
{
public:
    using Self = FunctionBlockTypeImpl;
    using Super = GenericComponentTypeImpl<IFunctionBlockType>;
    explicit FunctionBlockTypeImpl(const StringPtr& id,
                                   const StringPtr& name,
                                   const StringPtr& description,
                                   const FunctionPtr& createDefaultConfigCallback);

    // ISerializable
    ErrCode INTERFACE_FUNC serialize(ISerializer* serializer) override;
    ErrCode INTERFACE_FUNC getSerializeId(ConstCharPtr* id) const override;

    static ConstCharPtr SerializeId();
    static ErrCode Deserialize(ISerializedObject* serialized, IBaseObject* context, IFunction* factoryCallback, IBaseObject** obj);
};

OPENDAQ_REGISTER_DESERIALIZE_FACTORY(FunctionBlockTypeImpl)

END_NAMESPACE_OPENDAQ
