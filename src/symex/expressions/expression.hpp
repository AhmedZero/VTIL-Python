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
// | expression.hpp          | https://github.com/vtil-project/VTIL-Core      |
// |                         | https://github.com/pybind/pybind11             |
// |--------------------------------------------------------------------------|
//
#pragma once

#include <vtil/vtil>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

using namespace vtil::symbolic;
namespace py = pybind11;


namespace vtil::python
{
	class expression_reference_py : public py::class_<expression_reference>
	{
		public:
		expression_reference_py( const handle& scope, const char* name )
			: class_( scope, name )
		{
			( *this )

			.def( "is_valid", &expression_reference::is_valid )

			.def( "make_lazy", py::overload_cast<>(&expression_reference::make_lazy) )
			.def( "simplify", []( const expression::reference p, bool prettify ){ return p.simplify(prettify); }, 
				py::arg("prettify") = false )
			.def( "resize", []( expression_reference exp, bitcnt_t new_size, bool signed_cast, bool no_explicit ) { 
						return exp.resize( new_size, signed_cast, no_explicit );
				}, py::arg("new_size"), py::arg("signed_cast") = false, py::arg("no_explicit") = false )
			.def( "hash", &expression_reference::hash )
			.def( "is_simple", &expression_reference::is_simple )
			.def( "update", &expression_reference::update )

			.def( "equals", &expression_reference::equals )
			.def( "is_identical", &expression_reference::is_identical )
			.def( "size", &expression_reference::size )
			.def( "get_bool", [](const expression::reference p) { return *p->get<bool>(); })
			.def( "get_uint8", [](const expression::reference p) { return *p->get<uint8_t>(); })
			.def( "get_uint16", [](const expression::reference p) { return *p->get<uint16_t>(); })
			.def( "get_uint32", [](const expression::reference p) { return *p->get<uint32_t>(); })
			.def( "get_uint64", [](const expression::reference p) { return *p->get<uint64_t>(); })

#define IMPLEMENT_PROPERTY(name) .def_property_readonly( #name, [](expression_reference cls) { return cls->name; })
			IMPLEMENT_PROPERTY(uid)
			IMPLEMENT_PROPERTY(op)
			IMPLEMENT_PROPERTY(lhs)
			IMPLEMENT_PROPERTY(rhs)
			IMPLEMENT_PROPERTY(complexity)
			IMPLEMENT_PROPERTY(depth)
			IMPLEMENT_PROPERTY(hash_value)
			IMPLEMENT_PROPERTY(signature)
			IMPLEMENT_PROPERTY(simplify_hint)
			IMPLEMENT_PROPERTY(is_lazy)
			IMPLEMENT_PROPERTY(value)
#undef IMPLEMENT_PROPERTY

#define IMPLEMENT_FUNC(name) .def( #name, [](expression_reference cls) { return cls->name(); })
			IMPLEMENT_FUNC(get_op_desc)

			IMPLEMENT_FUNC(is_variable)
			IMPLEMENT_FUNC(is_expression)
			IMPLEMENT_FUNC(is_unary)
			IMPLEMENT_FUNC(is_binary)

			IMPLEMENT_FUNC(known_mask)
			IMPLEMENT_FUNC(unknown_mask)
			IMPLEMENT_FUNC(known_one)
			IMPLEMENT_FUNC(known_zero)
			IMPLEMENT_FUNC(is_constant)
#undef IMPLEMENT_FUNC
			.def( "evaluate", [](expression_reference cls, std::function<std::optional<uint64_t>(const vtil::symbolic::unique_identifier& uid)> lookup) 
			{
				vtil::math::bit_vector result;
				{
					py::gil_scoped_acquire acquire;
					result = cls->evaluate(lookup);
				}
				return result;
			}, pybind11::call_guard<pybind11::gil_scoped_release>())
			.def( "to_string", &expression_reference::to_string )
			.def( "__repr__", &expression_reference::to_string )
			.def( "__str__", &expression_reference::to_string )

			.def( "popcnt", [ ] ( expression_reference rhs ) { return __popcnt( rhs ); } )
			.def( "mask", [ ] ( expression_reference rhs ) { return __mask( rhs ); } )
			.def( "bsr", [ ] ( expression_reference rhs ) { return __bsr( rhs ); } )
			.def( "bsf", [ ] ( expression_reference rhs ) { return __bsf( rhs ); } )
			.def( "__invert__", [ ] ( expression_reference rhs ) { return ~rhs; } )
			.def( "__neg__", [ ] ( expression_reference rhs ) { return -rhs; } )

			// End
			//
			;

#define IMPLEMENT_OPERATOR( name, ... ) 															  	\
		.def( name, [ ] ( expression_reference lhs, expression_reference rhs ) { return __VA_ARGS__; } )\
		.def( name, [ ] ( expression_reference lhs, expression rhs ) { return __VA_ARGS__; } )	  		\
		.def( name, [ ] ( expression_reference lhs, int64_t rhs ) { return __VA_ARGS__; } )			  	\
		.def( name, [ ] ( expression_reference lhs, uint64_t rhs ) { return __VA_ARGS__; } )

			( *this )
				// Functions
				//
				IMPLEMENT_OPERATOR( "rotr", __rotr( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "rotl", __rotl( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "mulhi", mulhi( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "umulhi", umulhi( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "umul", umul( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "udiv", udiv( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "urem", urem( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "bt", __bt( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "iff", __if( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "max", __max( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "min", __min( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "umax", __umax( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "umin", __umin( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "ugt", __ugreat( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "uge", __ugreat_eq( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "ueq", __uequal( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "une", __unot_equal( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "ule", __uless_eq( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "ult", __uless( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "__add__", lhs + rhs )
				IMPLEMENT_OPERATOR( "__sub__", lhs - rhs )
				IMPLEMENT_OPERATOR( "__mul__", lhs * rhs )
				IMPLEMENT_OPERATOR( "__truediv__", lhs / rhs )
				IMPLEMENT_OPERATOR( "__mod__", lhs % rhs )
				IMPLEMENT_OPERATOR( "__and__", lhs & rhs )
				IMPLEMENT_OPERATOR( "__or__", lhs | rhs )
				IMPLEMENT_OPERATOR( "__xor__", lhs ^ rhs )
				IMPLEMENT_OPERATOR( "__lshift__", lhs << rhs )
				IMPLEMENT_OPERATOR( "__rshift__", lhs >> rhs )
				IMPLEMENT_OPERATOR( "__gt__", lhs > rhs )
				IMPLEMENT_OPERATOR( "__ge__", lhs >= rhs )
				IMPLEMENT_OPERATOR( "__lt__", lhs < rhs )
				IMPLEMENT_OPERATOR( "__le__", lhs <= rhs )
				IMPLEMENT_OPERATOR( "__eq__", lhs == rhs )
				IMPLEMENT_OPERATOR( "__ne__", lhs != rhs )

				// End
				//
				;
#undef IMPLEMENT_OPERATOR

#define IMPLEMENT_ROPERATOR( name, ... ) 															  	\
		.def( name, [ ] ( const expression_reference rhs, int64_t lhs ) { return __VA_ARGS__; } )		\
		.def( name, [ ] ( const expression_reference rhs, uint64_t lhs ) { return __VA_ARGS__; } )

			( *this )
				// Functions
				//
				IMPLEMENT_ROPERATOR( "__radd__", lhs + rhs )
				IMPLEMENT_ROPERATOR( "__rsub__", lhs - rhs )
				IMPLEMENT_ROPERATOR( "__rmul__", lhs* rhs )
				IMPLEMENT_ROPERATOR( "__rtruediv__", lhs / rhs )
				IMPLEMENT_ROPERATOR( "__rmod__", lhs% rhs )
				IMPLEMENT_ROPERATOR( "__rand__", lhs& rhs )
				IMPLEMENT_ROPERATOR( "__ror__", lhs | rhs )
				IMPLEMENT_ROPERATOR( "__rxor__", lhs^ rhs )
				IMPLEMENT_ROPERATOR( "__rlshift__", lhs << rhs )
				IMPLEMENT_ROPERATOR( "__rrshift__", lhs >> rhs )

				// End
				//
				;
#undef IMPLEMENT_ROPERATOR
		}
	};
	class expression_py : public py::class_<expression>
	{
		public:
		expression_py( const handle& scope, const char* name )
			: class_( scope, name )
		{
			( *this )
				// Constructor
				//
				.def( py::init<>() )
				.def( py::init<const expression&>() )
				.def( py::init<const uint64_t, bitcnt_t>(),
					py::arg("value"), py::arg("bit_count") = vtil::arch::bit_count )
				.def( py::init<const int64_t, bitcnt_t>(),
					py::arg("value"), py::arg("bit_count") = vtil::arch::bit_count )

				.def( py::init<const unique_identifier&, bitcnt_t>() )

				.def( py::init<math::operator_id, const expression::reference&>() )
				.def( py::init<const expression::reference&, math::operator_id, const expression::reference&>() )

				// Properties
				//
				.def_readwrite( "uid", &expression::uid )

				.def_readwrite( "op", &expression::op )
				.def_readwrite( "lhs", &expression::lhs )
				.def_readwrite( "rhs", &expression::rhs )

				.def_readwrite( "complexity", &expression::complexity )
				.def_readwrite( "depth", &expression::depth )
				.def_readwrite( "is_lazy", &expression::is_lazy )

				// Functions
				//
				.def( "get_op_desc", &expression::get_op_desc )

				.def( "is_variable", &expression::is_variable )
				.def( "is_expression", &expression::is_expression )
				.def( "is_unary", &expression::is_unary )
				.def( "is_binary", &expression::is_binary )
				.def( "is_valid", &expression::is_valid )

				.def( "hash", &expression::hash )
				.def( "count_constants", &expression::count_constants )
				.def( "count_variables", &expression::count_variables )
				.def( "count_unique_variables", &expression::count_unique_variables )

				.def( "update", &expression::update )
				.def( "to_string", &expression::to_string )

				.def( "resize", [ ] ( expression& exp, bitcnt_t new_size, bool signed_cast, bool no_explicit ) { 
						return exp.resize( new_size, signed_cast, no_explicit );
					}, py::arg("new_size"), py::arg("signed_cast") = false, py::arg("no_explicit") = false )
				.def( "simplify", py::overload_cast< bool >( &expression::simplify ), py::arg("prettify") = false )

				.def( "is_identical", &expression::is_identical )
				.def( "equals", &expression::equals )

				.def( "evaluate", [ ] ( expression& exp, std::function < uint64_t( unique_identifier )>& fn ) { exp.evaluate( fn ); } )
				.def( "make_lazy", py::overload_cast< >( &expression::make_lazy ) )

				.def( "get_int", [](const expression exp) { return *exp.get<true>(); })
				.def( "get_uint", [](const expression exp) { return *exp.get<false>(); })
				
#define IMPLEMENT_FUNC(name) .def( #name, [](expression cls) { return cls.name(); })
				IMPLEMENT_FUNC(known_mask)
				IMPLEMENT_FUNC(unknown_mask)
				IMPLEMENT_FUNC(known_one)
				IMPLEMENT_FUNC(known_zero)
				IMPLEMENT_FUNC(is_constant)
#undef IMPLEMENT_FUNC

				.def( "__repr__", &expression::to_string )
				.def( "__str__", &expression::to_string )

				.def( "popcnt", [ ] ( const expression& rhs ) { return __popcnt( rhs ); } )
				.def( "mask", [ ] ( const expression& rhs ) { return __mask( rhs ); } )
				.def( "bsr", [ ] ( const expression& rhs ) { return __bsr( rhs ); } )
				.def( "bsf", [ ] ( const expression& rhs ) { return __bsf( rhs ); } )
				.def( "__invert__", [ ] ( const expression& rhs ) { return ~rhs; } )
				.def( "__neg__", [ ] ( const expression& rhs ) { return -rhs; } )


				// End
				//
				;


#define IMPLEMENT_OPERATOR( name, ... ) 															  \
		.def( name, [ ] ( const expression& lhs, const expression& rhs ) { return __VA_ARGS__; } )	  \
		.def( name, [ ] ( const expression& lhs, int64_t rhs ) { return __VA_ARGS__; } )			  \
		.def( name, [ ] ( const expression& lhs, uint64_t rhs ) { return __VA_ARGS__; } )

			( *this )
				// Functions
				//
				IMPLEMENT_OPERATOR( "rotr", __rotr( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "rotl", __rotl( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "mulhi", mulhi( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "umulhi", umulhi( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "umul", umul( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "udiv", udiv( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "urem", urem( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "bt", __bt( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "iff", __if( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "max", __max( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "min", __min( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "umax", __umax( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "umin", __umin( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "ugt", __ugreat( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "uge", __ugreat_eq( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "ueq", __uequal( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "une", __unot_equal( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "ule", __uless_eq( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "ult", __uless( lhs, rhs ) )
				IMPLEMENT_OPERATOR( "__add__", lhs + rhs )
				IMPLEMENT_OPERATOR( "__sub__", lhs - rhs )
				IMPLEMENT_OPERATOR( "__mul__", lhs * rhs )
				IMPLEMENT_OPERATOR( "__truediv__", lhs / rhs )
				IMPLEMENT_OPERATOR( "__mod__", lhs % rhs )
				IMPLEMENT_OPERATOR( "__and__", lhs & rhs )
				IMPLEMENT_OPERATOR( "__or__", lhs | rhs )
				IMPLEMENT_OPERATOR( "__xor__", lhs ^ rhs )
				IMPLEMENT_OPERATOR( "__lshift__", lhs << rhs )
				IMPLEMENT_OPERATOR( "__rshift__", lhs >> rhs )
				IMPLEMENT_OPERATOR( "__gt__", lhs > rhs )
				IMPLEMENT_OPERATOR( "__ge__", lhs >= rhs )
				IMPLEMENT_OPERATOR( "__lt__", lhs < rhs )
				IMPLEMENT_OPERATOR( "__le__", lhs <= rhs )
				IMPLEMENT_OPERATOR( "__eq__", lhs == rhs )
				IMPLEMENT_OPERATOR( "__ne__", lhs != rhs )

				// End
				//
				;
#undef IMPLEMENT_OPERATOR

#define IMPLEMENT_ROPERATOR( name, ... ) 															  \
		.def( name, [ ] ( const expression& rhs, int64_t lhs ) { return __VA_ARGS__; } )			  \
		.def( name, [ ] ( const expression& rhs, uint64_t lhs ) { return __VA_ARGS__; } )

			( *this )
				// Functions
				//
				IMPLEMENT_ROPERATOR( "__radd__", lhs + rhs )
				IMPLEMENT_ROPERATOR( "__rsub__", lhs - rhs )
				IMPLEMENT_ROPERATOR( "__rmul__", lhs* rhs )
				IMPLEMENT_ROPERATOR( "__rtruediv__", lhs / rhs )
				IMPLEMENT_ROPERATOR( "__rmod__", lhs% rhs )
				IMPLEMENT_ROPERATOR( "__rand__", lhs& rhs )
				IMPLEMENT_ROPERATOR( "__ror__", lhs | rhs )
				IMPLEMENT_ROPERATOR( "__rxor__", lhs^ rhs )
				IMPLEMENT_ROPERATOR( "__rlshift__", lhs << rhs )
				IMPLEMENT_ROPERATOR( "__rrshift__", lhs >> rhs )

				// End
				//
				;
#undef IMPLEMENT_ROPERATOR
		}
	};
}
