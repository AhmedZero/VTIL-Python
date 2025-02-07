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
// | auxiliaries.hpp         | https://github.com/vtil-project/VTIL-Core      |
// |                         | https://github.com/pybind/pybind11             |
// |--------------------------------------------------------------------------|
//
#pragma once

#include <vtil/vtil>
#include <pybind11/pybind11.h>

using namespace vtil::optimizer::aux;
namespace py = pybind11;


namespace vtil::python
{
	class branch_analysis_flags_py : public py::class_<branch_analysis_flags>
	{
		public:
		branch_analysis_flags_py( const handle& scope, const char* name )
			: class_( scope, name )
		{
			( *this )
				// Constructor
				//
				.def( py::init<>() )
				.def( py::init( [](bool cross_block, bool pack, bool resolve_opaque) {
						auto p = branch_analysis_flags();
						p.cross_block = cross_block;
						p.pack = pack;
						p.resolve_opaque = resolve_opaque;
						return p; 
					} ),
					py::arg("cross_block") = false, py::arg("pack") = false, py::arg("resolve_opaque") = false )

				// Functions
				//
				.def_property( "cross_block", 
                    [](branch_analysis_flags &self) {return self.cross_block;}, 
                    [](branch_analysis_flags &self, bool v) {self.cross_block = v;})
				.def_property( "pack", 
                    [](branch_analysis_flags &self) {return self.pack;}, 
                    [](branch_analysis_flags &self, bool v) {self.pack = v;})
				.def_property( "resolve_opaque", 
                    [](branch_analysis_flags &self) {return self.resolve_opaque;}, 
                    [](branch_analysis_flags &self, bool v) {self.resolve_opaque = v;})

				// End
				//
				;
		}
	};

	class branch_info_py : public py::class_<branch_info>
	{
		public:
		branch_info_py( const handle& scope, const char* name )
			: class_( scope, name )
		{
			( *this )
				// Constructor
				//
				.def( py::init<>() )

				// Functions
				//
				.def_readwrite( "is_vm_exit", &vtil::optimizer::aux::branch_info::is_vm_exit)
				.def_readwrite( "is_jcc", &vtil::optimizer::aux::branch_info::is_jcc)
				.def_readwrite( "cc", &vtil::optimizer::aux::branch_info::cc)
				.def_readwrite( "destinations", &vtil::optimizer::aux::branch_info::destinations)

				// End
				//
				;
		}
	};

	struct dummy_aux {};
	class aux_py : public py::class_<dummy_aux>
	{
		public:
		aux_py( const handle& scope, const char* name )
			: class_( scope, name )
		{
			branch_analysis_flags_py( *this, "branch_analysis_flags" );
			branch_info_py( *this, "branch_info" );

			( *this )
				// Functions
				//
				.def_static("is_local", &vtil::optimizer::aux::is_local)
				.def_static("is_used", &vtil::optimizer::aux::is_used)
				.def_static("is_alive", &vtil::optimizer::aux::is_alive)
				.def_static("revive_register", &vtil::optimizer::aux::revive_register)
				.def_static("analyze_branch", &vtil::optimizer::aux::analyze_branch)
				.def_static("is_semantic_nop", &vtil::optimizer::aux::is_semantic_nop)
				.def_static("remove_nops", [](basic_block* blk, bool semantic_nops, bool volatile_nops) { return vtil::optimizer::aux::remove_nops(blk, semantic_nops, volatile_nops); })
				.def_static("remove_nops", [](routine* rtn, bool semantic_nops, bool volatile_nops) { return vtil::optimizer::aux::remove_nops(rtn, semantic_nops, volatile_nops); })

				// End
				//
				;
		}
	};

	

	struct dummy_optimizer {};
	class optimizer_py : public py::class_<dummy_optimizer>
	{
		public:
		static auto my_apply_all(vtil::routine* rtn) { return vtil::optimizer::apply_all(rtn); }
		static auto my_apply_all(vtil::basic_block* blk, bool xblock) { return vtil::optimizer::apply_all(blk, xblock); }

		static auto my_apply_all_profiled(vtil::routine* rtn) { return vtil::optimizer::apply_all_profiled(rtn); }
		static auto my_apply_all_profiled(vtil::basic_block* blk, bool xblock) { return vtil::optimizer::apply_all_profiled(blk, xblock); }

		template<typename T>
		static auto my_pass(vtil::routine* rtn) { return T{}(rtn); }
		template<typename T>
		static auto my_pass(vtil::basic_block* blk, bool xblock){ return T{}(blk, xblock); }

		optimizer_py( const handle& scope, const char* name )
			: class_( scope, name )
		{
			aux_py( *this, "aux");
			using namespace vtil::optimizer;

#define IMPLEMENT_PASS_X( name, pass ) \
				.def_static(name, py::overload_cast<vtil::routine*>(&my_pass<pass>)) \
				.def_static(name, py::overload_cast<vtil::basic_block*, bool>(&my_pass<pass>), \
					py::arg("blk"), py::arg("xblock") = false)

#define IMPLEMENT_PASS( name ) IMPLEMENT_PASS_X(#name, name)

			( *this )
				// Functions
				//
				.def_static("analyze_branch", &vtil::optimizer::aux::analyze_branch)
				.def_static("apply_all", py::overload_cast<vtil::routine*>(&my_apply_all))
				.def_static("apply_all", py::overload_cast<vtil::basic_block*, bool>(&my_apply_all))
				.def_static("apply_all_profiled", py::overload_cast<vtil::routine*>(&my_apply_all_profiled))
				.def_static("apply_all_profiled", py::overload_cast<vtil::basic_block*, bool>(&my_apply_all_profiled))
				IMPLEMENT_PASS(stack_pinning_pass)
				IMPLEMENT_PASS(istack_ref_substitution_pass)
				IMPLEMENT_PASS(bblock_extension_pass)
				IMPLEMENT_PASS(stack_propagation_pass)
				IMPLEMENT_PASS(dead_code_elimination_pass)
				IMPLEMENT_PASS(fast_dead_code_elimination_pass)
				IMPLEMENT_PASS(fast_reg_propagation_pass)
				IMPLEMENT_PASS(mov_propagation_pass)
				IMPLEMENT_PASS_X("symbolic_rewrite_pass_force", symbolic_rewrite_pass<true>)
				IMPLEMENT_PASS_X("symbolic_rewrite_pass", symbolic_rewrite_pass<false>)
				IMPLEMENT_PASS(branch_correction_pass)
				IMPLEMENT_PASS(register_renaming_pass)
				IMPLEMENT_PASS(collective_cross_pass)
				// End
				//
				;
#undef IMPLEMENT_PASS
#undef IMPLEMENT_PASS_X
		}
	};
}
