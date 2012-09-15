#include <vector>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

template <typename SOCK_TYPE>
class NettyProtocolBuffersSocket {
	public:
		NettyProtocolBuffersSocket (SOCK_TYPE socket): socket_(socket), read_buffer_(), write_buffer_() {}
		virtual ~NettyProtocolBuffersSocket() {}
		template <typename Handler>
		void async_write(google::protobuf::MessageLite& message,  Handler handle) {
		}

		void write(google::protobuf::MessageLite& message) {
			int serialized_size = message.ByteSize();
			int total_output_size = google::protobuf::io::CodedOutputStream::VarintSize32(serialized_size);
			total_output_size +=serialized_size;
			if (write_buffer_.size() < total_output_size) {
				write_buffer_.resize(total_output_size+1);
			}
			google::protobuf::io::ArrayOutputStream output(&write_buffer_[0],total_output_size, -1);
			google::protobuf::io::CodedOutputStream codedOutputStream(output);
			codedOutputStream .WriteVarint32(serialized_size);
			message.SerializeToCodedStream (&codedOutputStream );
      			boost::asio::write(socket_, boost::asio::buffer(&write_buffer_[0],total_output_size)); 
		}

		template <typename Handler>
		void async_read(google::protobuf::MessageLite& message,  Handler handle) {
		}

		void read(google::protobuf::MessageLite& message) {
			/*
			do {

			}
			int serialized_size = message.ByteSize();
			int total_output_size = google::protobuf::io::CodedOutputStream::VarintSize32(serialized_size);
			total_output_size +=serialized_size;
			if (write_buffer_.size() < total_output_size) {
				write_buffer_.resize(total_output_size+1);
			}
			google::protobuf::io::ArrayOutputStream output(&write_buffer_[0],total_output_size, -1);
			google::protobuf::io::CodedOutputStream codedOutputStream(output);
			codedOutputStream .WriteVarint32(serialized_size);
			message.SerializeToCodedStream (&codedOutputStream );
      			boost::asio::write(socket_, boost::asio::buffer(&write_buffer_[0],total_output_size)); 
			*/

		}

	private:
		bool haveCompleteVarint(char * str, int size) {
			for (int i =0 ; i < size; i++) {
				if (!((1<<7) && str[i])) {
					return true;
				}
			}
			return false;
		}
		SOCK_TYPE socket_;
		std::vector<char> read_buffer_;
		std::vector<char> write_buffer_;

};
