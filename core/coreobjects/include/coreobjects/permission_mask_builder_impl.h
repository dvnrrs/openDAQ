/*
 * Copyright 2022-2024 Blueberry d.o.o.
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
#include <coreobjects/permission_mask_builder.h>
#include <coretypes/intfs.h>

BEGIN_NAMESPACE_OPENDAQ

class PermissionMaskBuilderImpl final : public ImplementationOf<IPermissionMaskBuilder>
{
public:
    explicit PermissionMaskBuilderImpl(Int permissionMask);

    ErrCode INTERFACE_FUNC read() override;
    ErrCode INTERFACE_FUNC write() override;
    ErrCode INTERFACE_FUNC execute() override;
    ErrCode INTERFACE_FUNC clear() override;
    ErrCode INTERFACE_FUNC build(Int* permissionMask) override;

private:
    Int permissionMask;
};

END_NAMESPACE_OPENDAQ
