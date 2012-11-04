#pragma once
#pragma warning(disable : 4267 4996)

#include <streambuf>


class StreamSizeCounter : public std::basic_streambuf<char>
{
	std::streamsize total_size;
public:
	StreamSizeCounter()
	{
		total_size = 0;
	}

	std::streamsize GetSize() {return total_size; }

protected:
	virtual std::streamsize xsputn(const char_type*, std::streamsize count)
	{

		total_size += count;
		return count;
	}
};


class ArrayInputStream : public std::streambuf
{
public:
	ArrayInputStream(char* begin, char* end)
	{
		setg(begin, begin, end);
	}
	ArrayInputStream(char* begin, size_t size)
	{
		setg(begin, begin, begin + size);
	}

	int_type underflow()
	{
		return  gptr() == egptr() ?
				traits_type::eof() :
				traits_type::to_int_type(*gptr());
	}
};

class ArrayOutputStream : public std::streambuf
{
public:
	ArrayOutputStream(char* begin, char* end)
	{
		setp(begin, begin, end);
	}
	ArrayOutputStream(char* begin, size_t size)
	{
		setp(begin, begin, begin + size);
	}
};
