// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "test.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* TestMessage_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  TestMessage_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_test_2eproto() {
  protobuf_AddDesc_test_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "test.proto");
  GOOGLE_CHECK(file != NULL);
  TestMessage_descriptor_ = file->message_type(0);
  static const int TestMessage_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TestMessage, foo_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TestMessage, bar_),
  };
  TestMessage_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      TestMessage_descriptor_,
      TestMessage::default_instance_,
      TestMessage_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TestMessage, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TestMessage, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(TestMessage));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_test_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    TestMessage_descriptor_, &TestMessage::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_test_2eproto() {
  delete TestMessage::default_instance_;
  delete TestMessage_reflection_;
}

void protobuf_AddDesc_test_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\ntest.proto\"\'\n\013TestMessage\022\013\n\003foo\030\001 \002(\t"
    "\022\013\n\003bar\030\002 \001(\005", 53);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "test.proto", &protobuf_RegisterTypes);
  TestMessage::default_instance_ = new TestMessage();
  TestMessage::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_test_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_test_2eproto {
  StaticDescriptorInitializer_test_2eproto() {
    //protobuf_AddDesc_test_2eproto();
  }
} static_descriptor_initializer_test_2eproto_;


// ===================================================================

#ifndef _MSC_VER
const int TestMessage::kFooFieldNumber;
const int TestMessage::kBarFieldNumber;
#endif  // !_MSC_VER

TestMessage::TestMessage()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void TestMessage::InitAsDefaultInstance() {
}

TestMessage::TestMessage(const TestMessage& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void TestMessage::SharedCtor() {
  _cached_size_ = 0;
  foo_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  bar_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

TestMessage::~TestMessage() {
  SharedDtor();
}

void TestMessage::SharedDtor() {
  if (foo_ != &::google::protobuf::internal::kEmptyString) {
    delete foo_;
  }
  if (this != default_instance_) {
  }
}

void TestMessage::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* TestMessage::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return TestMessage_descriptor_;
}

const TestMessage& TestMessage::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_test_2eproto();  return *default_instance_;
}

TestMessage* TestMessage::default_instance_ = NULL;

TestMessage* TestMessage::New() const {
  return new TestMessage;
}

void TestMessage::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_foo()) {
      if (foo_ != &::google::protobuf::internal::kEmptyString) {
        foo_->clear();
      }
    }
    bar_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool TestMessage::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string foo = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_foo()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->foo().data(), this->foo().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_bar;
        break;
      }
      
      // optional int32 bar = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_bar:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &bar_)));
          set_has_bar();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void TestMessage::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string foo = 1;
  if (has_foo()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->foo().data(), this->foo().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->foo(), output);
  }
  
  // optional int32 bar = 2;
  if (has_bar()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->bar(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* TestMessage::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string foo = 1;
  if (has_foo()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->foo().data(), this->foo().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->foo(), target);
  }
  
  // optional int32 bar = 2;
  if (has_bar()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->bar(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int TestMessage::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string foo = 1;
    if (has_foo()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->foo());
    }
    
    // optional int32 bar = 2;
    if (has_bar()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->bar());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void TestMessage::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const TestMessage* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const TestMessage*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void TestMessage::MergeFrom(const TestMessage& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_foo()) {
      set_foo(from.foo());
    }
    if (from.has_bar()) {
      set_bar(from.bar());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void TestMessage::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void TestMessage::CopyFrom(const TestMessage& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool TestMessage::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void TestMessage::Swap(TestMessage* other) {
  if (other != this) {
    std::swap(foo_, other->foo_);
    std::swap(bar_, other->bar_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata TestMessage::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;

  //metadata.descriptor = TestMessage_descriptor_;
  //metadata.reflection = TestMessage_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
