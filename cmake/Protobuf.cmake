set(protobuf_PREFIX ${PROJECT_SOURCE_DIR}/lib/protobuf)
file(MAKE_DIRECTORY ${protobuf_PREFIX})

ExternalProject_Add(
    protobuf_ext

    URL https://github.com/protocolbuffers/protobuf/releases/download/v3.6.1/protobuf-cpp-3.6.1.tar.gz
    URL_HASH SHA256=b3732e471a9bb7950f090fd0457ebd2536a9ba0891b7f3785919c654fe2a2529

    PREFIX "protobuf"
    INSTALL_DIR ${protobuf_PREFIX}

    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix <INSTALL_DIR>
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Get_Property(protobuf_ext source_dir)
set(protobuf_SOURCE_DIR ${source_dir})

set(protobuf_SOURCE_FILES
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/bytestream.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/bytestream.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/common.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/hash.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/int128.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/int128.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/map_util.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/mathutil.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/status.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/status.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/status_macros.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/statusor.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/statusor.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/stringpiece.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/stringpiece.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/stringprintf.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/stringprintf.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/structurally_valid.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/strutil.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/time.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/time.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/arena.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/extension_set.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/generated_message_util.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/generated_message_table_driven_lite.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/generated_message_table_driven_lite.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/implicit_weak_message.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/message_lite.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/repeated_field.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/wire_format_lite.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/io/coded_stream.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/io/coded_stream_inl.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/io/strtod.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/io/zero_copy_stream.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/io/zero_copy_stream_impl_lite.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/any.pb.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/api.pb.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/mathlimits.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/mathlimits.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/any.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/descriptor.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/descriptor_database.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/descriptor.pb.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/duration.pb.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/dynamic_message.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/empty.pb.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/extension_set_heavy.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/field_mask.pb.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/generated_message_reflection.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/generated_message_table_driven_lite.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/generated_message_table_driven.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/map_field.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/message.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/reflection_internal.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/reflection_ops.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/service.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/source_context.pb.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/struct.pb.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/substitute.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/stubs/substitute.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/text_format.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/timestamp.pb.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/type.pb.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/unknown_field_set.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/wire_format.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/wrappers.pb.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/io/gzip_stream.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/io/printer.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/io/strtod.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/io/tokenizer.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/io/zero_copy_stream_impl.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/compiler/importer.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/compiler/parser.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/delimited_message_util.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/field_comparator.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/field_mask_util.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/constants.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/datapiece.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/datapiece.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/default_value_objectwriter.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/default_value_objectwriter.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/error_listener.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/error_listener.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/expecting_objectwriter.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/field_mask_utility.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/field_mask_utility.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/json_escaping.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/json_escaping.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/json_objectwriter.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/json_objectwriter.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/json_stream_parser.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/json_stream_parser.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/location_tracker.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/mock_error_listener.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/object_location_tracker.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/object_source.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/object_writer.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/object_writer.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/protostream_objectsource.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/protostream_objectsource.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/protostream_objectwriter.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/protostream_objectwriter.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/proto_writer.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/proto_writer.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/structured_objectwriter.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/type_info.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/type_info.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/type_info_test_helper.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/type_info_test_helper.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/utility.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/internal/utility.h
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/json_util.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/message_differencer.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/time_util.cc
    ${protobuf_SOURCE_DIR}/src/google/protobuf/util/type_resolver_util.cc
)
set_source_files_properties(${protobuf_SOURCE_FILES} PROPERTIES GENERATED TRUE)

file(GLOB_RECURSE protobuf_HEADER_FILES ${protobuf_SOURCE_DIR}/src/**/*.h)
include_directories(${protobuf_SOURCE_DIR}/src)

add_library(protobuf ${protobuf_SOURCE_FILES})
add_dependencies(protobuf protobuf_ext)
set_target_properties(
    protobuf
    PROPERTIES
    IMPORTED_CONFIGURATIONS Release
    INCLUDE_DIRECTORIES ${protobuf_SOURCE_DIR}/src
    PUBLIC_HEADER "${protobuf_HEADER_FILES}"
    LINK_FLAGS -no-undefined
)

target_compile_options(protobuf PRIVATE -DHAVE_PTHREAD=1)

install(TARGETS protobuf
    ARCHIVE DESTINATION ${protobuf_PREFIX}/lib
    PUBLIC_HEADER DESTINATION ${protobuf_PREFIX}/include
)
