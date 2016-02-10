//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/core/Variant.hh"
#include "pxl/core/Tokenizer.hh"

#include <algorithm>

namespace pxl
{

Variant::Variant() :
		type(TYPE_NONE)
{
}

Variant::~Variant()
{
	clear();
}

Variant::Variant(const Variant& a) :
		type(TYPE_NONE)
{
	copy(a);
}

Variant::Variant(const char *s)
{
	data._String = new std::string(s);
	type = TYPE_STRING;
}

Variant::Variant(const Serializable &a)
{
	data._Serializable = a.clone();
	type = TYPE_SERIALIZABLE;
}

Variant::Variant(const Serializable *a)
{
	data._Serializable = a->clone();
	type = TYPE_SERIALIZABLE;
}

Variant::Variant(const std::vector<Variant> &a)
{
	data._vec = new std::vector<Variant>(a);
	type = TYPE_VECTOR;
}

void Variant::clear()
{
	if (type == TYPE_STRING)
	{
		safe_delete(data._String);
	}
	else if (type == TYPE_BASIC3VECTOR)
	{
		safe_delete(data._Basic3Vector);
	}
	else if (type == TYPE_LORENTZVECTOR)
	{
		safe_delete(data._LorentzVector);
	}
	else if (type == TYPE_SERIALIZABLE)
	{
		safe_delete(data._Serializable);
	}
	else if (type == TYPE_VECTOR)
	{
		safe_delete(data._vec);
	}
	type = TYPE_NONE;
}

void Variant::check(const Type t) const
{
	if (type != t)
		throw bad_conversion(type, t);
}

void Variant::check(const Type t)
{
	if (type == TYPE_NONE)
	{
		memset(&data, 0, sizeof(data));
		switch (t)
		{
		case TYPE_STRING:
			data._String = new std::string;
			break;
		case TYPE_BASIC3VECTOR:
			data._Basic3Vector = new Basic3Vector;
			break;
		case TYPE_LORENTZVECTOR:
			data._LorentzVector = new LorentzVector;
			break;
		case TYPE_VECTOR:
			data._vec = new vector_t;
			break;
		default:
			break;
		}
		type = t;
	}
	else if (type != t)
	{
		throw bad_conversion(type, t);
	}
}

const std::type_info& Variant::getTypeInfo() const
{
	if (type == TYPE_BOOL)
	{
		const std::type_info &ti = typeid(data._Bool);
		return ti;
	}
	else if (type == TYPE_CHAR)
	{
		const std::type_info &ti = typeid(data._Char);
		return ti;
	}
	else if (type == TYPE_UCHAR)
	{
		const std::type_info &ti = typeid(data._UChar);
		return ti;
	}
	else if (type == TYPE_INT16)
	{
		const std::type_info &ti = typeid(data._Int16);
		return ti;
	}
	else if (type == TYPE_UINT16)
	{
		const std::type_info &ti = typeid(data._UInt16);
		return ti;
	}
	else if (type == TYPE_INT32)
	{
		const std::type_info &ti = typeid(data._Int32);
		return ti;
	}
	else if (type == TYPE_UINT32)
	{
		const std::type_info &ti = typeid(data._UInt32);
		return ti;
	}
	else if (type == TYPE_INT64)
	{
		const std::type_info &ti = typeid(data._Int64);
		return ti;
	}
	else if (type == TYPE_UINT64)
	{
		const std::type_info &ti = typeid(data._UInt64);
		return ti;
	}
	else if (type == TYPE_FLOAT)
	{
		const std::type_info &ti = typeid(data._Float);
		return ti;
	}
	else if (type == TYPE_DOUBLE)
	{
		const std::type_info &ti = typeid(data._Double);
		return ti;
	}
	else if (type == TYPE_STRING)
	{
		const std::type_info &ti = typeid(*data._String);
		return ti;
	}
	else if (type == TYPE_SERIALIZABLE)
	{
		const std::type_info &ti = typeid(data._Serializable);
		return ti;
	}
	else if (type == TYPE_BASIC3VECTOR)
	{
		const std::type_info &ti = typeid(data._Basic3Vector);
		return ti;
	}
	else if (type == TYPE_LORENTZVECTOR)
	{
		const std::type_info &ti = typeid(data._LorentzVector);
		return ti;
	}
	else if (type == TYPE_VECTOR)
	{
		const std::type_info &ti = typeid(*data._vec);
		return ti;
	}
	else
	{
		const std::type_info &ti = typeid(0);
		return ti;
	}

}

const char *Variant::getTypeName(Type type)
{
	if (type == TYPE_NONE)
	{
		return "none";
	}
	else if (type == TYPE_BOOL)
	{
		return "bool";
	}
	else if (type == TYPE_CHAR)
	{
		return "char";
	}
	else if (type == TYPE_UCHAR)
	{
		return "uchar";
	}
	else if (type == TYPE_INT16)
	{
		return "int16";
	}
	else if (type == TYPE_UINT16)
	{
		return "uint16";
	}
	else if (type == TYPE_INT32)
	{
		return "int32";
	}
	else if (type == TYPE_UINT32)
	{
		return "uint32";
	}
	else if (type == TYPE_INT64)
	{
		return "int64";
	}
	else if (type == TYPE_UINT64)
	{
		return "uint64";
	}
	else if (type == TYPE_FLOAT)
	{
		return "float";
	}
	else if (type == TYPE_DOUBLE)
	{
		return "double";
	}
	else if (type == TYPE_SERIALIZABLE)
	{
		return "Serializable";
	}
	else if (type == TYPE_BASIC3VECTOR)
	{
		return "Basic3Vector";
	}
	else if (type == TYPE_LORENTZVECTOR)
	{
		return "LorentzVector";
	}
	else if (type == TYPE_STRING)
	{
		return "string";
	}
	else if (type == TYPE_VECTOR)
	{
		return "vector";
	}
	else
	{
		return "unknown";
	}
}

Variant::Type Variant::toType(const std::string &name)
{
	if (name == "none")
	{
		return TYPE_NONE;
	}
	else if (name == "bool")
	{
		return TYPE_BOOL;
	}
	else if (name == "char")
	{
		return TYPE_CHAR;
	}
	else if (name == "uchar")
	{
		return TYPE_UCHAR;
	}
	else if (name == "int16")
	{
		return TYPE_INT16;
	}
	else if (name == "uint16")
	{
		return TYPE_UINT16;
	}
	else if (name == "int32")
	{
		return TYPE_INT32;
	}
	else if (name == "uint32")
	{
		return TYPE_UINT32;
	}
	else if (name == "int64")
	{
		return TYPE_INT64;
	}
	else if (name == "uint64")
	{
		return TYPE_UINT64;
	}
	else if (name == "float")
	{
		return TYPE_FLOAT;
	}
	else if (name == "double")
	{
		return TYPE_DOUBLE;
	}
	else if (name == "Serializable")
	{
		return TYPE_SERIALIZABLE;
	}
	else if (name == "Basic3Vector")
	{
		return TYPE_BASIC3VECTOR;
	}
	else if (name == "LorentzVector")
	{
		return TYPE_LORENTZVECTOR;
	}
	else if (name == "string")
	{
		return TYPE_STRING;
	}
	else if (name == "vector")
	{
		return TYPE_STRING;
	}
	else
	{
		return TYPE_NONE;
	}
}

bool Variant::operator ==(const Variant &a) const
{
	if (type != a.type)
		return false;
	if (type == TYPE_BOOL)
	{
		return (data._Bool == a.data._Bool);
	}
	else if (type == TYPE_CHAR)
	{
		return (data._Char == a.data._Char);
	}
	else if (type == TYPE_UCHAR)
	{
		return (data._UChar == a.data._UChar);
	}
	else if (type == TYPE_INT16)
	{
		return (data._Int16 == a.data._Int16);
	}
	else if (type == TYPE_UINT16)
	{
		return (data._UInt16 == a.data._UInt16);
	}
	else if (type == TYPE_INT32)
	{
		return (data._Int32 == a.data._Int32);
	}
	else if (type == TYPE_UINT32)
	{
		return (data._UInt32 == a.data._UInt32);
	}
	else if (type == TYPE_INT64)
	{
		return (data._Int64 == a.data._Int64);
	}
	else if (type == TYPE_UINT64)
	{
		return (data._UInt64 == a.data._UInt64);
	}
	else if (type == TYPE_FLOAT)
	{
		return (data._Float == a.data._Float);
	}
	else if (type == TYPE_DOUBLE)
	{
		return (data._Double == a.data._Double);
	}
	else if (type == TYPE_SERIALIZABLE)
	{
		return (data._Serializable == a.data._Serializable);
	}
	else if (type == TYPE_BASIC3VECTOR)
	{
		return ((*data._Basic3Vector) == (*a.data._Basic3Vector));
	}
	else if (type == TYPE_STRING)
	{
		return (*data._String == *a.data._String);
	}
	else if (type == TYPE_VECTOR)
	{
		return (*data._vec == *a.data._vec);
	}
	else
	{
		throw std::runtime_error("compare operator not implemented");
	}
}

std::string Variant::toString() const
{
	if (type == TYPE_STRING)
		return *data._String;

	std::stringstream sstr;
	if (type == TYPE_BOOL)
	{
		sstr << data._Bool;
	}
	else if (type == TYPE_CHAR)
	{
		sstr << data._Char;
	}
	else if (type == TYPE_UCHAR)
	{
		sstr << data._UChar;
	}
	else if (type == TYPE_INT16)
	{
		sstr << data._Int16;
	}
	else if (type == TYPE_UINT16)
	{
		sstr << data._UInt16;
	}
	else if (type == TYPE_INT32)
	{
		sstr << data._Int32;
	}
	else if (type == TYPE_UINT32)
	{
		sstr << data._UInt32;
	}
	else if (type == TYPE_INT64)
	{
		sstr << data._Int64;
	}
	else if (type == TYPE_UINT64)
	{
		sstr << data._UInt64;
	}
	else if (type == TYPE_FLOAT)
	{
		sstr << data._Float;
	}
	else if (type == TYPE_DOUBLE)
	{
		sstr << data._Double;
	}
	else if (type == TYPE_BASIC3VECTOR)
	{
		sstr << data._Basic3Vector->getX() << " " << data._Basic3Vector->getY()
				<< " " << data._Basic3Vector->getZ();
	}
	else if (type == TYPE_LORENTZVECTOR)
	{
		sstr << data._LorentzVector->getX() << " "
				<< data._LorentzVector->getY() << " "
				<< data._LorentzVector->getZ() << " "
				<< data._LorentzVector->getE();
	}
	else if (type == TYPE_VECTOR)
	{
		sstr << *data._vec;
	}

	return sstr.str();
}

Variant Variant::fromString(const std::string &str, Type type)
{
	std::stringstream sstr(str);
	switch (type)
	{
	case TYPE_BOOL:
	{
		std::string upperstr(str);
		std::transform(upperstr.begin(), upperstr.end(), upperstr.begin(),
				(int(*)(int))toupper);if
(		upperstr == "YES")
		return Variant(true);
		else if (upperstr == "NO")
		return Variant(false);
		if (upperstr == "TRUE")
			return Variant(true);
		else if (upperstr == "FALSE")
			return Variant(false);
		if (upperstr == "1")
			return Variant(true);
		else if (upperstr == "0")
			return Variant(false);
		throw bad_conversion(type, TYPE_BOOL);
	}
	case TYPE_CHAR:
	{
		char c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_UCHAR:
	{
		unsigned char c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_INT16:
	{
		int16_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_UINT16:
	{
		uint16_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_INT32:
	{
		int32_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_UINT32:
	{
		uint32_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_INT64:
	{
		int64_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_UINT64:
	{
		uint64_t c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_FLOAT:
	{
		float c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_DOUBLE:
	{
		double c;
		sstr >> c;
		return Variant(c);
	}
	case TYPE_BASIC3VECTOR:
	{
		double d;
		Basic3Vector v;
		sstr >> d;
		v.setX(d);
		sstr >> d;
		v.setY(d);
		sstr >> d;
		v.setZ(d);
		return Variant(v);
	}
	case TYPE_LORENTZVECTOR:
	{
		double d;
		LorentzVector v;
		sstr >> d;
		v.setX(d);
		sstr >> d;
		v.setY(d);
		sstr >> d;
		v.setZ(d);
		sstr >> d;
		v.setE(d);
		return Variant(v);
	}
	case TYPE_STRING:
	{
		return Variant(str);
	}
	case TYPE_VECTOR:
	{
		std::vector<Variant> stringVectorValue;
		Tokenizer tok;
		tok.setText(str);
		tok.setCharType('[', Tokenizer::WHITESPACE);
		tok.setCharType(']', Tokenizer::WHITESPACE);
		tok.setCharType('(', Tokenizer::WHITESPACE);
		tok.setCharType(')', Tokenizer::WHITESPACE);
		tok.setCharType('\"', Tokenizer::LITERAL);
		tok.setCharType('\'', Tokenizer::LITERAL);
		tok.setCharType(',', Tokenizer::DELIM);
		while (tok.hasNext())
		{
			std::string s = tok.next();
			stringVectorValue.push_back(s);
		}
		return Variant(stringVectorValue);
	}
	default:
		throw std::runtime_error("pxl::Variant::fromString: unknown type");
	}

}

bool Variant::operator !=(const Variant &a) const
{
	if (type != a.type)
		return true;

	switch (type)
	{
	case TYPE_BOOL:
		return (data._Bool != a.data._Bool);
	case TYPE_CHAR:
		return (data._Char != a.data._Char);
	case TYPE_UCHAR:
		return (data._UChar != a.data._UChar);
	case TYPE_INT16:
		return (data._Int16 != a.data._Int16);
	case TYPE_UINT16:
		return (data._UInt16 == a.data._UInt16);
	case TYPE_INT32:
		return (data._Int32 == a.data._Int32);
	case TYPE_UINT32:
		return (data._UInt32 == a.data._UInt32);
	case TYPE_INT64:
		return (data._Int64 == a.data._Int64);
	case TYPE_UINT64:
		return (data._UInt64 == a.data._UInt64);
	case TYPE_FLOAT:
		return (data._Float == a.data._Float);
	case TYPE_DOUBLE:
		return (data._Double == a.data._Double);
	case TYPE_SERIALIZABLE:
		return (data._Serializable == a.data._Serializable);
	case TYPE_BASIC3VECTOR:
		return ((*data._Basic3Vector) == (*a.data._Basic3Vector));
	case TYPE_STRING:
		return (*data._String == *a.data._String);
	case TYPE_VECTOR:
		return (*data._vec != *a.data._vec);
	default:
		throw std::runtime_error("compare operator not implemented");
	}
}

void Variant::serialize(const OutputStream &out) const
{
	out.write((char) type);
	switch (type)
	{
	case TYPE_BOOL:
		out.write(data._Bool);
		break;
	case TYPE_CHAR:
		out.write(data._Char);
		break;
	case TYPE_UCHAR:
		out.write(data._UChar);
		break;
	case TYPE_INT16:
		out.write(data._Int16);
		break;
	case TYPE_UINT16:
		out.write(data._UInt16);
		break;
	case TYPE_INT32:
		out.write(data._Int32);
		break;
	case TYPE_UINT32:
		out.write(data._UInt32);
		break;
	case TYPE_INT64:
		out.write(data._Int64);
		break;
	case TYPE_UINT64:
		out.write(data._UInt64);
		break;
	case TYPE_FLOAT:
		out.writeFloat(data._Float);
		break;
	case TYPE_DOUBLE:
		out.writeDouble(data._Double);
		break;
	case TYPE_STRING:
		out.writeString(*data._String);
		break;
	case TYPE_BASIC3VECTOR:
		data._Basic3Vector->serialize(out);
		break;
	case TYPE_LORENTZVECTOR:
		data._LorentzVector->serialize(out);
		break;
	case TYPE_VECTOR:
	{
		uint32_t s = data._vec->size();
		out.write(s);
		for (size_t i = 0; i < s; i++)
			data._vec->at(i).serialize(out);
		break;
	}
	default:
		break;
	}
}

void Variant::deserialize(const InputStream &in)
{
	unsigned char t;
	in.read(t);
	check(static_cast<Type>(t));

	switch (type)
	{
	case TYPE_BOOL:
		in.readBool(data._Bool);
		break;
	case TYPE_CHAR:
		in.read(data._Char);
		break;
	case TYPE_UCHAR:
		in.read(data._UChar);
		break;
	case TYPE_INT16:
		in.read(data._Int16);
		break;
	case TYPE_UINT16:
		in.read(data._UInt16);
		break;
	case TYPE_INT32:
		in.read(data._Int32);
		break;
	case TYPE_UINT32:
		in.read(data._UInt32);
		break;
	case TYPE_INT64:
		in.read(data._Int64);
		break;
	case TYPE_UINT64:
		in.read(data._UInt64);
		break;
	case TYPE_FLOAT:
		in.readFloat(data._Float);
		break;
	case TYPE_DOUBLE:
		in.readDouble(data._Double);
		break;
	case TYPE_STRING:
		in.readString(*data._String);
		break;
	case TYPE_BASIC3VECTOR:
		data._Basic3Vector->deserialize(in);
		break;
	case TYPE_LORENTZVECTOR:
		data._LorentzVector->deserialize(in);
		break;
	case TYPE_VECTOR:
	{
		uint32_t s = 0;
		in.read(s);
		data._vec->resize(s);
		for (size_t i = 0; i < s; i++)
			data._vec->at(i).deserialize(in);
		break;
	}
	default:
		break;
	}
}

void Variant::copy(const Variant &a)
{
	Type t = a.type;
	if (t == TYPE_BOOL)
	{
		operator =(a.data._Bool);
	}
	else if (t == TYPE_CHAR)
	{
		operator =(a.data._Char);
	}
	else if (t == TYPE_UCHAR)
	{
		operator =(a.data._UChar);
	}
	else if (t == TYPE_INT16)
	{
		operator =(a.data._Int16);
	}
	else if (t == TYPE_UINT16)
	{
		operator =(a.data._UInt16);
	}
	else if (t == TYPE_INT32)
	{
		operator =(a.data._Int32);
	}
	else if (t == TYPE_UINT32)
	{
		operator =(a.data._UInt32);
	}
	else if (t == TYPE_INT64)
	{
		operator =(a.data._Int64);
	}
	else if (t == TYPE_UINT64)
	{
		operator =(a.data._UInt64);
	}
	else if (t == TYPE_FLOAT)
	{
		operator =(a.data._Float);
	}
	else if (t == TYPE_DOUBLE)
	{
		operator =(a.data._Double);
	}
	else if (t == TYPE_STRING)
	{
		operator =(*a.data._String);
	}
	else if (t == TYPE_SERIALIZABLE)
	{
		operator =(a.data._Serializable);
	}
	else if (t == TYPE_BASIC3VECTOR)
	{
		operator =(*a.data._Basic3Vector);
	}
	else if (t == TYPE_LORENTZVECTOR)
	{
		operator =(*a.data._LorentzVector);
	}
	else if (t == TYPE_VECTOR)
	{
		operator =(*a.data._vec);
	}
	else
	{
		type = TYPE_NONE;
	}
}

bool Variant::toBool() const
{
	switch (type)
	{
	case TYPE_BOOL:
		return data._Bool;
		break;
	case TYPE_CHAR:
		return data._Char != 0;
		break;
	case TYPE_UCHAR:
		return data._UChar != 0;
		break;
	case TYPE_INT16:
		return data._Int16 != 0;
		break;
	case TYPE_UINT16:
		return data._UInt16 != 0;
		break;
	case TYPE_INT32:
		return data._Int32 != 0;
		break;
	case TYPE_UINT32:
		return data._UInt32 != 0;
		break;
	case TYPE_INT64:
		return data._Int64 != 0;
		break;
	case TYPE_UINT64:
		return data._UInt64 != 0;
		break;
	case TYPE_SERIALIZABLE:
		return data._Serializable != 0;
		break;
	case TYPE_STRING:
	{
		std::string upperstr(*data._String);
		std::transform(upperstr.begin(), upperstr.end(), upperstr.begin(),
				(int(*)(int))toupper);if
(		upperstr == "YES")
		return true;
		else if (upperstr == "NO")
		return false;
		if (upperstr == "TRUE")
			return true;
		else if (upperstr == "FALSE")
			return false;
		if (upperstr == "1")
			return true;
		else if (upperstr == "0")
			return false;
		else
			throw bad_conversion(type, TYPE_BOOL);
	}
		break;
	case TYPE_VECTOR:
		return data._vec->size() != 0;
		break;
	case TYPE_FLOAT:
	case TYPE_DOUBLE:
	case TYPE_BASIC3VECTOR:
	case TYPE_LORENTZVECTOR:
	case TYPE_NONE:
		throw bad_conversion(type, TYPE_BOOL);
		break;
	}
	return false;
}

#define INT_CASE(from_var, from_type, to_type, to) \
	case Variant::from_type:\
		if (data._##from_var < std::numeric_limits<to>::min() || data._##from_var > std::numeric_limits<to>::max())\
			throw bad_conversion(type, to_type);\
		else\
			return static_cast<to>(data._##from_var);\
		break;\

#define INT_FUNCTION(to_type, fun, to) \
to Variant::fun() const { \
	switch (type) { \
	case Variant::TYPE_BOOL: \
		return data._Bool ? 1 : 0; \
		break; \
	INT_CASE(Char, TYPE_CHAR, to_type, to) \
	INT_CASE(UChar, TYPE_UCHAR, to_type, to) \
	INT_CASE(Int16, TYPE_INT16, to_type, to) \
	INT_CASE(UInt16, TYPE_UINT16, to_type, to) \
	INT_CASE(Int32, TYPE_INT32, to_type, to) \
	INT_CASE(UInt32, TYPE_UINT32, to_type, to) \
	INT_CASE(Int64, TYPE_INT64, to_type, to) \
	INT_CASE(UInt64, TYPE_UINT64, to_type, to) \
	INT_CASE(Float, TYPE_FLOAT, to_type, to) \
	INT_CASE(Double, TYPE_DOUBLE, to_type, to) \
	case Variant::TYPE_STRING: \
		{ \
		long l = atol(data._String->c_str()); \
		if (l < std::numeric_limits<to>::min() || l > std::numeric_limits<to>::max()) \
			throw bad_conversion(type, to_type); \
		else \
			return l; \
		} \
		break; \
	case Variant::TYPE_SERIALIZABLE: \
	case Variant::TYPE_BASIC3VECTOR: \
	case Variant::TYPE_LORENTZVECTOR: \
	case Variant::TYPE_VECTOR: \
	case Variant::TYPE_NONE: \
		throw bad_conversion(type, TYPE_INT16); \
		break;\
	}\
	return 0;\
}

INT_FUNCTION( TYPE_CHAR, toChar, char)
INT_FUNCTION( TYPE_UCHAR, toUChar, unsigned char)
INT_FUNCTION( TYPE_INT16, toInt16, int16_t)
INT_FUNCTION( TYPE_UINT16, toUInt16, uint16_t)
INT_FUNCTION( TYPE_INT32, toInt32, int32_t)
INT_FUNCTION( TYPE_UINT32, toUInt32, uint32_t)
INT_FUNCTION( TYPE_INT64, toInt64, int64_t)
INT_FUNCTION( TYPE_UINT64, toUInt64, uint64_t)

PXL_DLL_EXPORT std::ostream& operator <<(std::ostream& os, const Variant &v)
{
	switch (v.getType())
	{
	case Variant::TYPE_BOOL:
		os << v.asBool();
		break;
	case Variant::TYPE_CHAR:
		os << v.asChar();
		break;
	case Variant::TYPE_UCHAR:
		os << v.asUChar();
		break;
	case Variant::TYPE_INT16:
		os << v.asInt16();
		break;
	case Variant::TYPE_UINT16:
		os << v.asUInt16();
		break;
	case Variant::TYPE_INT32:
		os << v.asInt32();
		break;
	case Variant::TYPE_UINT32:
		os << v.asUInt32();
		break;
	case Variant::TYPE_INT64:
		os << v.asInt64();
		break;
	case Variant::TYPE_UINT64:
		os << v.asUInt64();
		break;
	case Variant::TYPE_FLOAT:
		os << v.asFloat();
		break;
	case Variant::TYPE_DOUBLE:
		os << v.asDouble();
		break;
	case Variant::TYPE_STRING:
		os << v.asString();
		break;
	case Variant::TYPE_SERIALIZABLE:
		v.asSerializable().print(0, os);
		break;
	case Variant::TYPE_BASIC3VECTOR:
		os << v.asBasic3Vector();
		break;
	case Variant::TYPE_VECTOR:
	{
		const std::vector<Variant> &vec = v.asVector();
		os << "(";
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (i != 0)
				os << ", ";
			os << vec[i];
		}
		os << ")";
		break;
	}
	default:
		break;
	}
	return os;
}

// std::vector
bool Variant::isVector() const
{
	return (type == TYPE_VECTOR);
}

Variant::operator std::vector<Variant> &()
{
	check(TYPE_VECTOR);
	return *data._vec;
}

Variant::operator const std::vector<Variant> &() const
{
	check(TYPE_VECTOR);
	return *data._vec;
}

std::vector<Variant> &Variant::asVector()
{
	check(TYPE_VECTOR);
	return *data._vec;
}

const std::vector<Variant> &Variant::asVector() const
{
	check(TYPE_VECTOR);
	return *data._vec;
}

Variant Variant::fromVector(const std::vector<Variant> &s)
{
	return Variant(s);
}

Variant &Variant::operator =(const std::vector<Variant> &a)
{
	if (type != TYPE_VECTOR)
	{
		clear();
	}
	data._vec = new std::vector<Variant>(a);
	type = TYPE_VECTOR;
	return *this;
}

bool Variant::operator ==(const std::vector<Variant> &a) const
{
	if (type != TYPE_VECTOR)
		return false;
	return *data._vec == a;
}

bool Variant::operator !=(const std::vector<Variant> &a) const
{
	check(TYPE_VECTOR);
	return *data._vec != a;
}

Variant &Variant::operator[](size_t i)
{
	check(TYPE_VECTOR);
	return (*data._vec)[i];
}

const Variant &Variant::operator[](size_t i) const
{
	check(TYPE_VECTOR);
	return (*data._vec)[i];
}

void Variant::resize(size_t i)
{
	check(TYPE_VECTOR);
	return data._vec->resize(i);
}

float Variant::toFloat() const
{
	if (type == TYPE_CHAR)
	{
		return static_cast<float>(data._Char);
	}
	else if (type == TYPE_UCHAR)
	{
		return static_cast<float>(data._UChar);
	}
	else if (type == TYPE_INT16)
	{
		return static_cast<float>(data._Int16);
	}
	else if (type == TYPE_UINT16)
	{
		return static_cast<float>(data._UInt16);
	}
	else if (type == TYPE_INT32)
	{
		return static_cast<float>(data._Int32);
	}
	else if (type == TYPE_UINT32)
	{
		return static_cast<float>(data._UInt32);
	}
	else if (type == TYPE_INT64)
	{
		return static_cast<float>(data._Int64);
	}
	else if (type == TYPE_UINT64)
	{
		return static_cast<float>(data._UInt64);
	}
	else if (type == TYPE_FLOAT)
	{
		return static_cast<float>(data._Float);
	}
	else if (type == TYPE_DOUBLE)
	{
		return static_cast<float>(data._Double);
	}
	else if (type == TYPE_STRING)
	{
		return static_cast<float>(std::atof(data._String->c_str()));
	}
	else if (type == TYPE_BOOL)
	{
		return data._Bool ? 1.0f : 0.0f;
	}
	else
	{
		return 0.0;
	}
}

double Variant::toDouble() const
{
	if (type == TYPE_CHAR)
	{
		return static_cast<double>(data._Char);
	}
	else if (type == TYPE_UCHAR)
	{
		return static_cast<double>(data._UChar);
	}
	else if (type == TYPE_INT16)
	{
		return static_cast<double>(data._Int16);
	}
	else if (type == TYPE_UINT16)
	{
		return static_cast<double>(data._UInt16);
	}
	else if (type == TYPE_INT32)
	{
		return static_cast<double>(data._Int32);
	}
	else if (type == TYPE_UINT32)
	{
		return static_cast<double>(data._UInt32);
	}
	else if (type == TYPE_INT64)
	{
		return static_cast<double>(data._Int64);
	}
	else if (type == TYPE_UINT64)
	{
		return static_cast<double>(data._UInt64);
	}
	else if (type == TYPE_FLOAT)
	{
		return static_cast<double>(data._Float);
	}
	else if (type == TYPE_DOUBLE)
	{
		return static_cast<double>(data._Double);
	}
	else if (type == TYPE_STRING)
	{
		return std::atof(data._String->c_str());
	}
	else if (type == TYPE_BOOL)
	{
		return data._Bool ? 1.0 : 0.0;
	}
	else
	{
		return 0.0;
	}
}

} // namespace pxl
