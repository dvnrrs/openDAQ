//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//
//     RTGen (PythonGenerator).
// </auto-generated>
//------------------------------------------------------------------------------

/*
 * Copyright 2022-2024 openDAQ d.o.o.
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

#include "py_opendaq/py_opendaq.h"
#include "py_core_types/py_converter.h"
#include "py_core_objects/py_variant_extractor.h"

PyDaqIntf<daq::IBlockReaderStatus, daq::IReaderStatus> declareIBlockReaderStatus(pybind11::module_ m)
{
    return wrapInterface<daq::IBlockReaderStatus, daq::IReaderStatus>(m, "IBlockReaderStatus");
}

void defineIBlockReaderStatus(pybind11::module_ m, PyDaqIntf<daq::IBlockReaderStatus, daq::IReaderStatus> cls)
{
    cls.doc() = "IBlockReaderStatus inherits from IReaderStatus to expand information returned read function";

    m.def("BlockReaderStatus", [](daq::IEventPacket* eventPacket, const bool valid, std::variant<daq::INumber*, double, daq::IEvalValue*>& offset, const size_t readSamples){
        return daq::BlockReaderStatus_Create(eventPacket, valid, getVariantValue<daq::INumber*>(offset), readSamples);
    }, py::arg("event_packet"), py::arg("valid"), py::arg("offset"), py::arg("read_samples"));


    cls.def_property_readonly("read_samples",
        [](daq::IBlockReaderStatus *object)
        {
            const auto objectPtr = daq::BlockReaderStatusPtr::Borrow(object);
            return objectPtr.getReadSamples();
        },
        "Returns the number of samples that were read. Sometimes, during the process of reading, an event packet may occur that stops the reading of remaining samples. Developers can use this function to determine how many samples were actually read.");
}
