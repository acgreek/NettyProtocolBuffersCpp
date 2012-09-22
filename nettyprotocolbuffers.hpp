#include <vector>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

static bool isCompleteVarInt(const char * varint, size_t buffer_size, size_t &offset_to_data) {
	offset_to_data = 0;
	while (offset_to_data != buffer_size) {
		unsigned char c = varint[offset_to_data];
		offset_to_data++;
		if ((c& (1<<7)) == 0)  {
			return true;
		}
	}
	return false;

}
static bool isCompleteVarInt(boost::asio::streambuf &buffer, size_t &offset_to_data){ 
	const char * varint = boost::asio::buffer_cast<const char *> (buffer.data());
	return isCompleteVarInt(varint, buffer.size(), offset_to_data);
}

template <typename SOCK_TYPE>
class NettyProtocolBuffersSocket {
	public:
		NettyProtocolBuffersSocket (SOCK_TYPE & socket): socket_(socket), read_buffer_(), write_buffer_() {}
		virtual ~NettyProtocolBuffersSocket() {}
		template <typename Handler>
		void async_write(google::protobuf::MessageLite& message,  Handler handle) {
		}

		void write(google::protobuf::MessageLite& message) {
			int serialized_size = message.ByteSize();
			size_t total_output_size = google::protobuf::io::CodedOutputStream::VarintSize32(serialized_size);
			total_output_size +=serialized_size;
			if (write_buffer_.size() < total_output_size) {
				write_buffer_.resize(total_output_size+1);
			}
			google::protobuf::io::ArrayOutputStream output(&write_buffer_[0],total_output_size, -1);
			google::protobuf::io::CodedOutputStream codedOutputStream(&output);
			codedOutputStream .WriteVarint32(serialized_size);
			message.SerializeToCodedStream (&codedOutputStream );
      			boost::asio::write(socket_, boost::asio::buffer(&write_buffer_[0],total_output_size)); 
		}

		template <typename Handler>
		void async_read(google::protobuf::MessageLite& message,  Handler handle) {
		}
		void read(google::protobuf::MessageLite& message) {
			size_t offset_to_data=0;
			
			do { //need to test this
				boost::asio::read(socket_,read_buffer_, boost::asio::transfer_at_least(1));
			} while (!isCompleteVarInt(read_buffer_, offset_to_data));
			const google::protobuf::uint8 * varint= boost::asio::buffer_cast<const google::protobuf::uint8 *> (read_buffer_.data());
			google::protobuf::io::CodedInputStream codedInputStream((google::protobuf::uint8 *) varint,offset_to_data) ;
			google::protobuf::uint32 data_size;
			codedInputStream.ReadVarint32(&data_size);
			read_buffer_.consume(offset_to_data);
			if (read_buffer_.size() < data_size) {
#if BOOST_VERSION < 104801
				boost::asio::read(socket_,read_buffer_, boost::asio::transfer_at_least(data_size- read_buffer_.size()));
#else 
				boost::asio::read(socket_,read_buffer_, boost::asio::transfer_exactly(data_size- read_buffer_.size()));
#endif 
			}
			varint= boost::asio::buffer_cast<const google::protobuf::uint8 *> (read_buffer_.data());
			message.ParseFromArray(varint, data_size);
			read_buffer_.consume(data_size);

		}

	private:
		SOCK_TYPE & socket_;
		boost::asio::streambuf read_buffer_;
		std::vector<char> write_buffer_;

};
