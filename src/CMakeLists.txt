add_library(resource_core STATIC
        src/document_builder.cpp
        src/inverted_index.cpp
)

target_include_directories(resource_core PUBLIC include)
