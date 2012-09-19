#include <vector>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

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
			int total_output_size = google::protobuf::io::CodedOutputStream::VarintSize32(serialized_size);
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
		bool isCompleteVarInt(boost::asio::streambuf &buffer, int & offset_to_data){ 
			const char * varint = boost::asio::buffer_cast<const char *> (buffer.data());
			offset_to_data = 0;
			while (offset_to_data != buffer.size()) {
				unsigned char c = varint[offset_to_data];
				offset_to_data++;
				std::cout << "c= " << (int)c << "\n";
				if ((c& (1<<7)) == 0)  {
					std::cout << "truy\n";
			
					return true;
				}
			}
			return false;
		}

		void read(google::protobuf::MessageLite& message) {
			int offset_to_data=0;
			
			do { //need to test this
				std::cout << "s read\n";
				size_t size =  boost::asio::read(socket_,read_buffer_, boost::asio::transfer_at_least(1));
				std::cout << "read  " << size << "\n";
			} while (!isCompleteVarInt(read_buffer_, offset_to_data));
			const google::protobuf::uint8 * varint= boost::asio::buffer_cast<const google::protobuf::uint8 *> (read_buffer_.data());
		
			std::cout << "offset " << offset_to_data << "\n";
			google::protobuf::io::CodedInputStream codedInputStream((google::protobuf::uint8 *) varint,offset_to_data) ;
			google::protobuf::uint32 data_size;
			bool success = codedInputStream.ReadVarint32(&data_size);
			std::cout << "success " <<success << "data_szie " << data_size << "\n";
			read_buffer_.consume(offset_to_data);
			if (read_buffer_.size() < data_size)
				boost::asio::read(socket_,read_buffer_, boost::asio::transfer_exactly(offset_to_data));
			varint= boost::asio::buffer_cast<const google::protobuf::uint8 *> (read_buffer_.data());
			message.ParseFromArray(varint, data_size);

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
		SOCK_TYPE & socket_;
		std::vector<char> write_buffer_;
		boost::asio::streambuf read_buffer_;

};
