#include <vector>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>

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
__attribute__((unused)) static bool isCompleteVarInt(boost::asio::streambuf &buffer, size_t &offset_to_data){ 
	const char * varint = boost::asio::buffer_cast<const char *> (buffer.data());
	return isCompleteVarInt(varint, buffer.size(), offset_to_data);
}

template <typename SOCK_TYPE>
class NettyProtocolBuffersSocket {
	public:
		NettyProtocolBuffersSocket (SOCK_TYPE & socket): socket_(socket), read_buffer_(), write_buffer_() {}
		virtual ~NettyProtocolBuffersSocket() {}
		
		template <typename Handler>
		void async_write_complete(const boost::system::error_code& e,std::size_t bytes_transferred, boost::tuple<Handler> handle) {
			boost::get<0>(handle)(e,bytes_transferred);
		}
			
		template <typename Handler>
		void async_write(google::protobuf::MessageLite& message,  Handler handle) {
			size_t total_output_size = serializeForWrite(message);
			void (NettyProtocolBuffersSocket<SOCK_TYPE>::*f1)(const boost::system::error_code&,std::size_t offset,boost::tuple<Handler>) =&NettyProtocolBuffersSocket<SOCK_TYPE>::async_write_complete <Handler> ;
			boost::asio::async_write(socket_,boost::asio::buffer(&write_buffer_[0],total_output_size), boost::asio::transfer_at_least(total_output_size), boost::bind(f1,this,boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, boost::make_tuple(handle)));

      			//boost::asio::async_write(socket_, boost::asio::buffer(&write_buffer_[0],total_output_size)); 
		}

		void write(google::protobuf::MessageLite& message) {
			size_t total_output_size = serializeForWrite(message);
      			boost::asio::write(socket_, boost::asio::buffer(&write_buffer_[0],total_output_size)); 
		}
		template <typename Handler>
		void async_read_rest(const boost::system::error_code& e,std::size_t varint_size,std::size_t message_size, google::protobuf::MessageLite& message, boost::tuple<Handler> handle) {
			if (e) {
				boost::get<0>(handle)(e,varint_size+message_size);
			}
			if (read_buffer_.size() < message_size) {
				void (NettyProtocolBuffersSocket<SOCK_TYPE>::*f1)(const boost::system::error_code&,std::size_t varint_size,std::size_t message_size,google::protobuf::MessageLite &mess,boost::tuple<Handler>) =&NettyProtocolBuffersSocket<SOCK_TYPE>::async_read_rest <Handler> ;
				boost::asio::async_read(socket_,read_buffer_, boost::asio::transfer_at_least(message_size -read_buffer_.size() ), boost::bind(f1,this,boost::asio::placeholders::error, varint_size,message_size, boost::ref(message),handle));
				return;
			}
			const google::protobuf::uint8 *varint= boost::asio::buffer_cast<const google::protobuf::uint8 *> (read_buffer_.data());
			message.ParseFromArray(varint, message_size);
			read_buffer_.consume(message_size);
			boost::get<0>(handle)(e, message_size);
		}
		template <typename Handler>
		void async_read_varint(const boost::system::error_code& e,std::size_t bytes_read, google::protobuf::MessageLite& message, boost::tuple<Handler> handle) {
			if (e) {
				boost::get<0>(handle)(e, bytes_read);
			}
			size_t offset_to_data=0;
			if (!isCompleteVarInt(read_buffer_, offset_to_data))  {
				void (NettyProtocolBuffersSocket<SOCK_TYPE>::*f1)(const boost::system::error_code&,std::size_t offset,google::protobuf::MessageLite &mess,boost::tuple<Handler>) =&NettyProtocolBuffersSocket<SOCK_TYPE>::async_read_varint <Handler> ;
				boost::asio::async_read(socket_,read_buffer_, boost::asio::transfer_at_least(1), boost::bind(f1,this,boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, boost::ref(message),handle));


				return;
			}
			const google::protobuf::uint8 * varint= boost::asio::buffer_cast<const google::protobuf::uint8 *> (read_buffer_.data());
			google::protobuf::io::CodedInputStream codedInputStream((google::protobuf::uint8 *) varint,offset_to_data) ;
			google::protobuf::uint32 data_size;
			codedInputStream.ReadVarint32(&data_size);
			read_buffer_.consume(offset_to_data);
			async_read_rest(e,offset_to_data,data_size, message, handle);
		}

		template <typename Handler>
		void async_read(google::protobuf::MessageLite& message,  Handler handle) {
			void (NettyProtocolBuffersSocket<SOCK_TYPE>::*f1)(const boost::system::error_code&,std::size_t offset,google::protobuf::MessageLite &mess,boost::tuple<Handler>) =&NettyProtocolBuffersSocket<SOCK_TYPE>::async_read_varint <Handler> ;
			boost::asio::async_read(socket_,read_buffer_, boost::asio::transfer_at_least(1), boost::bind(f1,this,boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, boost::ref(message),boost::make_tuple(handle)));
		}
		void read(google::protobuf::MessageLite& message) {
			boost::system::error_code ec;
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
		size_t serializeForWrite(google::protobuf::MessageLite& message) {
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
			return total_output_size;

		}
		SOCK_TYPE & socket_;
		boost::asio::streambuf read_buffer_;
		std::vector<char> write_buffer_;

};
