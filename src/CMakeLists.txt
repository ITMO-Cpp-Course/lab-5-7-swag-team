add_library(resource_core STATIC
    src/document_builder.cpp
    src/inverted_index.cpp
    src/text_utils.cpp
)

target_include_directories(resource_core PUBLIC include)

target_compile_features(resource_core PUBLIC cxx_std_23)

target_link_libraries(resource_core PRIVATE project_warnings)

if (TARGET project_sanitizers)
    target_link_libraries(resource_core PRIVATE project_sanitizers)
endif()