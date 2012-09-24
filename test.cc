#include <ExtremeCUnit.h>

#include "test.pb.h"
#include "nettyprotocolbuffers.hpp"
#include <list>
class DummySocket {
	public:
		DummySocket():list_bytes_for_write_(),list_bytes_for_read_()  {
		}
		template<typename ConstBufferSequence>
		std::size_t write_some( const ConstBufferSequence & buffers) {
			char * ptr = (char *)boost::asio::detail::buffer_cast_helper(buffers);
			return 1;
		}
		template< typename ConstBufferSequence, typename ReadHandler>
		void async_write_some(const ConstBufferSequence & buffers, ReadHandler handler) {
			boost::system::error_code ec;
			size_t bytes_written= write_some(buffers, ec);
			handler(ec, bytes_written);
		}
		template< typename ConstBufferSequence>
		std::size_t write_some(const ConstBufferSequence & buffers, boost::system::error_code & ec) {
			char * ptr = (char *)boost::asio::detail::buffer_cast_helper(*buffers.begin());
			std::size_t size= boost::asio::detail::buffer_size_helper(*buffers.begin());
			std::vector<char> vec(size);
			memcpy(&vec[0], ptr, size);
			list_bytes_for_read_.push_back(vec);
			return 1;
		}
		template <typename MutableBufferSequence, typename ReadHandler>
		void async_read_some(const MutableBufferSequence& buffers, ReadHandler handler) {
			boost::system::error_code ec;
			std::size_t bytes_written = read_some(buffers,ec);
			handler(ec, bytes_written);
		}
		void push_read_bytes(std::vector<char > bytes) {
			list_bytes_for_write_.push_back (bytes);
		}

		
		template<typename MutableBufferSequence>
		std::size_t read_some( const MutableBufferSequence & buffers,
				        boost::system::error_code & ec) {
			if (list_bytes_for_write_.size() == 0 ) {
				return 1;
			}
			std::vector<char >  bytes_to_write = list_bytes_for_write_.front();
			list_bytes_for_write_.pop_front();
			char * ptr = (char *)boost::asio::detail::buffer_cast_helper(buffers);
			std::size_t size = boost::asio::detail::buffer_size_helper(buffers);
			size_t write_offset=0;
			size_t write_this_time = size < bytes_to_write.size()-write_offset?size :bytes_to_write.size()-write_offset ;
			memcpy(ptr,&bytes_to_write [write_offset], write_this_time );
			if (size < bytes_to_write.size()) {
				std::vector<char> rest(bytes_to_write.begin() + size,bytes_to_write.end());
				list_bytes_for_write_.push_front(rest);
			}
			return write_this_time;
		}
		std::vector<char> getReadBuffer() {
			std::vector<char> b = list_bytes_for_read_.front();
			list_bytes_for_read_.pop_front();
			return b;

		}
		int read_buffers_remaining() {
			return list_bytes_for_write_.size();
		}

		std::list<std::vector<char > >  list_bytes_for_write_;
		std::list<std::vector<char > >  list_bytes_for_read_;

};

TEST(WriteTest_basic) {
	DummySocket dummySocket;
	NettyProtocolBuffersSocket<DummySocket> nettypbserializer(dummySocket); 
    	TestMessage input;
    	input.set_foo("blabal");
   	nettypbserializer.write(input);
	std::vector<char> b = dummySocket.getReadBuffer();
	AssertEqInt(9, (int)b.size());
	return 0;
}
int foo_called=0;
void foo(const boost::system::error_code& err,std::size_t size) {foo_called++; }

TEST(AsyncWriteTest_basic) {
	DummySocket dummySocket;
	NettyProtocolBuffersSocket<DummySocket> nettypbserializer(dummySocket); 
    	TestMessage input;
    	input.set_foo("blabal");
	AssertEqInt(foo_called, 0);
   	nettypbserializer.async_write(input,boost::bind(&foo, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ));
	AssertEqInt(foo_called, 1);
	std::vector<char> b = dummySocket.getReadBuffer();
	AssertEqInt(9, (int)b.size());
	
	return 0;
}
TEST(ReadTest_one) {
	DummySocket dummySocket;
	NettyProtocolBuffersSocket<DummySocket> nettypbserializer(dummySocket); 
    	TestMessage input;
    	input.set_foo("blabal");
	size_t msize = input.ByteSize();
	AssertEqInt(msize, 8);
	std::vector<char > data(9);
	data[0] = 8;
	google::protobuf::io::ArrayOutputStream output(&data[1],8, -1);
	google::protobuf::io::CodedOutputStream codedOutputStream(&output);
	input.SerializeToCodedStream (&codedOutputStream );
    	TestMessage output_mes;
	dummySocket.push_read_bytes(data);
   	nettypbserializer.read(output_mes);
	AssertEqStr(output_mes.foo().c_str(), "blabal");;
	return 0;
}

void read_foo(const boost::system::error_code& err,std::size_t size) {
	foo_called++;
}
TEST(AsyncReadTest_one) {
	DummySocket dummySocket;
	NettyProtocolBuffersSocket<DummySocket> nettypbserializer(dummySocket); 
    	TestMessage input;
    	input.set_foo("blabal");
	size_t msize = input.ByteSize();
	AssertEqInt(msize, 8);
	std::vector<char > data(9);
	data[0] = 8;
	google::protobuf::io::ArrayOutputStream output(&data[1],8, -1);
	google::protobuf::io::CodedOutputStream codedOutputStream(&output);
	input.SerializeToCodedStream (&codedOutputStream );
    	TestMessage output_mes;
	dummySocket.push_read_bytes(data);
   	nettypbserializer.async_read(output_mes,read_foo);
	AssertEqStr(output_mes.foo().c_str(), "blabal");;
	return 0;
}
TEST(AsyncReadTest_two_different_large) {
	DummySocket dummySocket;
	NettyProtocolBuffersSocket<DummySocket> nettypbserializer(dummySocket); 
    	TestMessage input;
    	input.set_foo("blabskdfjaldfjaslkdjfaslkdjfasdlkjfasldfjalksdfjlksadflkasdflkasjdfklasdasghdfgjksdgfhjksdfhgjksdfhgkjsdhfgjsfgkjadfklajsdflkjasdkfjasdklfjaklsdfjkalsdfjaklsdjflkasdjfklasdjflkasdjlkfasjdflkasjdlkfjasdlfkjasdlkfjalsdkjflaksdfjlkasdjflkasdfjlkasdjflksjdflkajdflkajsdflkjasdkfljasdlfjasdlkfjaslkdjfaskldjfalksdfjadkfajsdkfjaslkdfjaskdlfjaskldjfalskdjflkasdjflaksdjflaksdfjlkasdjfklasdfjlkasdjfklasdjflkasdjflkasdjflkasdjflkasdjfalksdjfalksdjfalskdjflkasdjflaksdjflkasdjflkasdjfalblabskdfjaldfjaslkdjfaslkdjfasdlkjfasldfjalksdfjlksadflkasdflkasjdfklasdasghdfgjksdgfhjksdfhgjksdfhgkjsdhfgjsfgkjadfklajsdflkjasdkfjasdklfjaklsdfjkalsdfjaklsdjflkasdjfklasdjflkasdjlkfasjdflkasjdlkfjasdlfkjasdlkfjalsdkjflaksdfjlkasdjflkasdfjlkasdjflksjdflkajdflkajsdflkjasdkfljasdlfjasdlkfjaslkdjfaskldjfalksdfjadkfajsdkfjaslkdfjaskdlfjaskldjfalskdjflkasdjflaksdjflaksdfjlkasdjfklasdfjlkasdjfklasdjflkasdjflkasdjflkasdjflkasdjfalksdjfalksdjfalskdjflkasdjflaksdjflkasdjflkasdjfal");
	std::vector<char > data(9800);
	google::protobuf::io::ArrayOutputStream output(&data[0],9800, -1);
	google::protobuf::io::CodedOutputStream codedOutputStream(&output);
	codedOutputStream.WriteVarint32( input.ByteSize());
	input.SerializeToCodedStream (&codedOutputStream );
    	input.set_foo("flasd");
	codedOutputStream.WriteVarint32( input.ByteSize());
	input.SerializeToCodedStream (&codedOutputStream );
    	TestMessage output_mes;
	dummySocket.push_read_bytes(data);
	//dummySocket.push_read_bytes(data);
	//dummySocket.push_read_bytes(data);
   	nettypbserializer.async_read(output_mes, read_foo);
	AssertEqInt(output_mes.foo().size(), 954);;
    	TestMessage output2_mes;
   	nettypbserializer.async_read(output2_mes,read_foo);
	AssertEqStr(output2_mes.foo().c_str(), "flasd");;
	AssertEqInt(dummySocket.read_buffers_remaining(), 1)
	return 0;
}
TEST(ReadTest_two) {
	DummySocket dummySocket;
	NettyProtocolBuffersSocket<DummySocket> nettypbserializer(dummySocket); 
    	TestMessage input;
    	input.set_foo("blabal");
	size_t msize = input.ByteSize();
	AssertEqInt(msize, 8);
	std::vector<char > data(9);
	data[0] = 8;
	google::protobuf::io::ArrayOutputStream output(&data[1],8, -1);
	google::protobuf::io::CodedOutputStream codedOutputStream(&output);
	input.SerializeToCodedStream (&codedOutputStream );
    	TestMessage output_mes;
	dummySocket.push_read_bytes(data);
	dummySocket.push_read_bytes(data);
	dummySocket.push_read_bytes(data);
   	nettypbserializer.read(output_mes);
	AssertEqStr(output_mes.foo().c_str(), "blabal");;
    	TestMessage output2_mes;
   	nettypbserializer.read(output2_mes);
	AssertEqStr(output2_mes.foo().c_str(), "blabal");;
	AssertEqInt(dummySocket.read_buffers_remaining(), 1)
	return 0;
}
TEST(ReadTest_two_different_large) {
	DummySocket dummySocket;
	NettyProtocolBuffersSocket<DummySocket> nettypbserializer(dummySocket); 
    	TestMessage input;
    	input.set_foo("blabskdfjaldfjaslkdjfaslkdjfasdlkjfasldfjalksdfjlksadflkasdflkasjdfklasdasghdfgjksdgfhjksdfhgjksdfhgkjsdhfgjsfgkjadfklajsdflkjasdkfjasdklfjaklsdfjkalsdfjaklsdjflkasdjfklasdjflkasdjlkfasjdflkasjdlkfjasdlfkjasdlkfjalsdkjflaksdfjlkasdjflkasdfjlkasdjflksjdflkajdflkajsdflkjasdkfljasdlfjasdlkfjaslkdjfaskldjfalksdfjadkfajsdkfjaslkdfjaskdlfjaskldjfalskdjflkasdjflaksdjflaksdfjlkasdjfklasdfjlkasdjfklasdjflkasdjflkasdjflkasdjflkasdjfalksdjfalksdjfalskdjflkasdjflaksdjflkasdjflkasdjfalblabskdfjaldfjaslkdjfaslkdjfasdlkjfasldfjalksdfjlksadflkasdflkasjdfklasdasghdfgjksdgfhjksdfhgjksdfhgkjsdhfgjsfgkjadfklajsdflkjasdkfjasdklfjaklsdfjkalsdfjaklsdjflkasdjfklasdjflkasdjlkfasjdflkasjdlkfjasdlfkjasdlkfjalsdkjflaksdfjlkasdjflkasdfjlkasdjflksjdflkajdflkajsdflkjasdkfljasdlfjasdlkfjaslkdjfaskldjfalksdfjadkfajsdkfjaslkdfjaskdlfjaskldjfalskdjflkasdjflaksdjflaksdfjlkasdjfklasdfjlkasdjfklasdjflkasdjflkasdjflkasdjflkasdjfalksdjfalksdjfalskdjflkasdjflaksdjflkasdjflkasdjfal");
	std::vector<char > data(9800);
	google::protobuf::io::ArrayOutputStream output(&data[0],9800, -1);
	google::protobuf::io::CodedOutputStream codedOutputStream(&output);
	codedOutputStream.WriteVarint32( input.ByteSize());
	input.SerializeToCodedStream (&codedOutputStream );
    	input.set_foo("flasd");
	codedOutputStream.WriteVarint32( input.ByteSize());
	input.SerializeToCodedStream (&codedOutputStream );
    	TestMessage output_mes;
	dummySocket.push_read_bytes(data);
	//dummySocket.push_read_bytes(data);
	//dummySocket.push_read_bytes(data);
   	nettypbserializer.read(output_mes);
	AssertEqInt(output_mes.foo().size(), 954);;
    	TestMessage output2_mes;
   	nettypbserializer.read(output2_mes);
	AssertEqStr(output2_mes.foo().c_str(), "flasd");;
	AssertEqInt(dummySocket.read_buffers_remaining(), 1)
	return 0;
}
TEST(ReadTest_two_different) {
	DummySocket dummySocket;
	NettyProtocolBuffersSocket<DummySocket> nettypbserializer(dummySocket); 
    	TestMessage input;
    	input.set_foo("blabal");
	size_t msize = input.ByteSize();
	AssertEqInt(msize, 8);
	std::vector<char > data(80);
	google::protobuf::io::ArrayOutputStream output(&data[0],80, -1);
	google::protobuf::io::CodedOutputStream codedOutputStream(&output);
	codedOutputStream.WriteVarint32( input.ByteSize());
	input.SerializeToCodedStream (&codedOutputStream );
    	input.set_foo("flasd");
	codedOutputStream.WriteVarint32( input.ByteSize());
	input.SerializeToCodedStream (&codedOutputStream );
    	TestMessage output_mes;
	dummySocket.push_read_bytes(data);
	dummySocket.push_read_bytes(data);
	dummySocket.push_read_bytes(data);
   	nettypbserializer.read(output_mes);
	AssertEqStr(output_mes.foo().c_str(), "blabal");;
    	TestMessage output2_mes;
   	nettypbserializer.read(output2_mes);
	AssertEqStr(output2_mes.foo().c_str(), "flasd");;
	AssertEqInt(dummySocket.read_buffers_remaining(), 1)
	return 0;
}
TEST(ReadTest_two_small) {
	DummySocket dummySocket;
	NettyProtocolBuffersSocket<DummySocket> nettypbserializer(dummySocket); 
    	TestMessage input;
    	input.set_foo("b");
	size_t msize = input.ByteSize();
	AssertEqInt(msize, 3);
	std::vector<char > data(11);
	google::protobuf::io::ArrayOutputStream output(&data[0],11, -1);
	google::protobuf::io::CodedOutputStream codedOutputStream(&output);
	codedOutputStream.WriteVarint32(3);
	input.SerializeToCodedStream (&codedOutputStream );
    	input.set_foo("a");
	codedOutputStream.WriteVarint32(3);
	input.SerializeToCodedStream (&codedOutputStream );
	codedOutputStream.WriteVarint32(3);
	input.SerializeToCodedStream (&codedOutputStream );
    	TestMessage output_mes;
	dummySocket.push_read_bytes(data);
	dummySocket.push_read_bytes(data);
	dummySocket.push_read_bytes(data);
   	nettypbserializer.read(output_mes);
	AssertEqStr(output_mes.foo().c_str(), "b");;
    	TestMessage output2_mes;
   	nettypbserializer.read(output2_mes);
	AssertEqStr(output2_mes.foo().c_str(), "a");;
	AssertEqInt(dummySocket.read_buffers_remaining(), 1)
	return 0;
}
TEST(VarInt_empty ) {
	size_t offset;
	Assert(false == isCompleteVarInt("", 0,  offset));
	return 0;
}
TEST(VarInt_not_complete ) {
	size_t offset;
	Assert(false == isCompleteVarInt("\xff", 1,  offset));
	return 0;
}
TEST(VarInt_complete_two_char  ) {
	size_t offset;
	Assert(true == isCompleteVarInt("\xff\000", 2,  offset));
	return 0;
}
