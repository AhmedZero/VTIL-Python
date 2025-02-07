// Copyright (c) 2020 Daniel (@L33T) and contributors of the VTIL Project   
// All rights reserved.   
//    
// Redistribution and use in source and binary forms, with or without   
// modification, are permitted provided that the following conditions are met: 
//    
// 1. Redistributions of source code must retain the above copyright notice,   
//    this list of conditions and the following disclaimer.   
// 2. Redistributions in binary form must reproduce the above copyright   
//    notice, this list of conditions and the following disclaimer in the   
//    documentation and/or other materials provided with the distribution.   
// 3. Neither the name of VTIL Project nor the names of its contributors
//    may be used to endorse or promote products derived from this software 
//    without specific prior written permission.   
//    
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE   
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR   
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF   
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS   
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN   
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)   
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  
// POSSIBILITY OF SUCH DAMAGE.        
//

// Furthermore, the following pieces of software have additional copyrights
// licenses, and/or restrictions:
//
// |--------------------------------------------------------------------------|
// | File name               | Link for further information                   |
// |-------------------------|------------------------------------------------|
// | register_desc.hpp       | https://github.com/vtil-project/VTIL-Core      |
// |                         | https://github.com/pybind/pybind11             |
// |--------------------------------------------------------------------------|
//
#pragma once

#include <vtil/vtil>
#include <pybind11/pybind11.h>

using namespace vtil;
namespace py = pybind11;


namespace vtil::python
{
	class register_desc_py : public py::class_<register_desc>
	{
		public:
		register_desc_py( const handle& scope, const char* name )
			: class_( scope, name )
		{
			py::enum_<register_flag>( scope, "register_flag" )
				.value( "register_virtual", register_flag::register_virtual )
				.value( "register_physical", register_flag::register_physical )
				.value( "register_local", register_flag::register_local )
				.value( "register_flags", register_flag::register_flags )
				.value( "register_stack_pointer", register_flag::register_stack_pointer )
				.value( "register_image_base", register_flag::register_image_base )
				.value( "register_volatile", register_flag::register_volatile )
				.value( "register_readonly", register_flag::register_readonly )
				.value( "register_undefined", register_flag::register_undefined )
				.value( "register_internal", register_flag::register_internal )
				.value( "register_special", register_flag::register_special )
				;

			( *this )
				.def( py::init<>() )
				.def( py::init<uint32_t, uint64_t, bitcnt_t, bitcnt_t, uint64_t>(),
					py::arg("flags"), py::arg("id"), py::arg("bit_count"), py::arg("bit_offset") = 0, py::arg("architecture") = 0 )

				.def( py::self != py::self )
				.def( py::self == py::self )
				// Static
				//

				// Properties
				//
				.def_readwrite( "flags", &register_desc::flags )
				.def_readwrite( "combined_id", &register_desc::combined_id )
				.def_readwrite( "bit_count", &register_desc::bit_count )
				.def_readwrite( "bit_offset", &register_desc::bit_offset )

				.def_property( "local_id", 
                    [](register_desc &self) {return self.local_id;}, 
                    [](register_desc &self, uint64_t v) {self.local_id = v;})
				.def_property( "architecture", 
                    [](register_desc &self) {return self.architecture;}, 
                    [](register_desc &self, uint64_t v) {self.architecture = v;})
				
				// Functions
				//
				.def( "is_valid", &register_desc::is_valid )
				.def( "is_flags", &register_desc::is_flags )
				.def( "is_undefined", &register_desc::is_undefined )
				.def( "is_local", &register_desc::is_local )
				.def( "is_global", &register_desc::is_global )
				.def( "is_virtual", &register_desc::is_virtual )
				.def( "is_physical", &register_desc::is_physical )
				.def( "is_volatile", &register_desc::is_volatile )
				.def( "is_read_only", &register_desc::is_read_only )
				.def( "is_stack_pointer", &register_desc::is_stack_pointer )
				.def( "is_image_base", &register_desc::is_image_base )
				.def( "is_special", &register_desc::is_special )
				.def( "is_internal", &register_desc::is_internal )

				.def( "get_mask", &register_desc::get_mask )
				.def( "overlaps", &register_desc::overlaps )
				.def( "select", &register_desc::select )
				.def( "rebase", &register_desc::rebase )
				.def( "resize", &register_desc::resize )
				.def( "to_string", &register_desc::to_string )

				.def( "reduce", py::overload_cast< >( &register_desc::reduce ) )

				.def( "__repr__", &register_desc::to_string )
				.def( "__str__", &register_desc::to_string )

				// End
				//
				;
		}
	};
}
