#include <ExtremeCUnit.h>

#include "test.pb.h"
#include "nettyprotocolbuffers.hpp"
#include <list>
class DummySocket {
	public:
		DummySocket():list_bytes_for_write_() {
		}
		template<
			typename ConstBufferSequence>
		std::size_t write_some( const ConstBufferSequence & buffers) {
	
			return 1;
		}
		template< typename ConstBufferSequence>
		std::size_t write_some(const ConstBufferSequence & buffers, boost::system::error_code & ec) {
			return 1;
		}
		void push_read_bytes(std::vector<char > bytes) {
			list_bytes_for_write_.push_back (bytes);
		}

		std::list<std::vector<char > >  list_bytes_for_write_;
		
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
			return write_this_time;
		}

};

TEST(WriteTest_basic) {
	DummySocket dummySocket;
	NettyProtocolBuffersSocket<DummySocket> nettypbserializer(dummySocket); 
    	TestMessage input;
    	input.set_foo("blabal");
   	nettypbserializer.write(input);
	return 0;
}
TEST(ReadTest_one_by_one) {
	DummySocket dummySocket;
	NettyProtocolBuffersSocket<DummySocket> nettypbserializer(dummySocket); 
    	TestMessage input;
    	input.set_foo("blabal");
	size_t msize = input.ByteSize();
	AssertEqInt(msize, 8);
	std::vector<char > data(9);
	data[0] = 8;
	google::protobuf::io::ArrayOutputStream output(&data[0],8, -1);
	google::protobuf::io::CodedOutputStream codedOutputStream(&output);
	input.SerializeToCodedStream (&codedOutputStream );
    	TestMessage output_mes;
	dummySocket.push_read_bytes(data);
   	nettypbserializer.read(output_mes);
	AssertEqStr(output_mes.foo().c_str(), "blabal");;
	return 0;
}

