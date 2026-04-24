#include <catch2/catch_all.hpp>

#include "document_builder.hpp"
#include "inverted_index.hpp"

#include <algorithm>

using namespace lab5::index;

// DocumentBuilder

TEST_CASE("DocumentBuilder: tokenization: simple ", "[builder]")
{
    DocumentBuilder builder;
    Document doc = builder.build("doc", "Hello World");

    REQUIRE(doc.name == "doc");
    REQUIRE(doc.id == 0);
    REQUIRE(doc.words == std::vector<std::string>{"hello", "world"});
}

TEST_CASE("DocumentBuilder: tokenization: punctuation is stripped", "[builder]")
{
    DocumentBuilder builder;
    Document doc = builder.build("doc", "Hello, World!");

    REQUIRE(doc.words == std::vector<std::string>{"hello", "world"});
}

TEST_CASE("DocumentBuilder: tokenization: lowercase check", "[builder]")
{
    DocumentBuilder builder;
    Document doc = builder.build("doc", "FOO BAR Baz");

    REQUIRE(doc.words == std::vector<std::string>{"foo", "bar", "baz"});
}

TEST_CASE("DocumentBuilder: tokenization: hyphenated word is preserved", "[builder]")
{
    DocumentBuilder builder;
    Document doc = builder.build("doc", "well-known fact");

    REQUIRE(doc.words == std::vector<std::string>{"well-known", "fact"});
}

TEST_CASE("DocumentBuilder: empty text produces empty words", "[builder]")
{
    DocumentBuilder builder;
    Document doc = builder.build("doc", "");

    REQUIRE(doc.words.empty());
}

// InvertedIndex: add

TEST_CASE("InvertedIndex: add: simple", "[index][add]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    REQUIRE(idx.add(builder.build("doc1", "hello world")));
}

TEST_CASE("InvertedIndex: add: rejects duplicate name", "[index][add]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));
    REQUIRE_FALSE(idx.add(builder.build("doc1", "other text")));
}

TEST_CASE("InvertedIndex: add: rejects empty name", "[index][add]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    REQUIRE_FALSE(idx.add(builder.build("", "hello world")));
}

TEST_CASE("InvertedIndex: add: rejects document with no words", "[index][add]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    REQUIRE_FALSE(idx.add(builder.build("doc1", "")));
}

// InvertedIndex: search

TEST_CASE("InvertedIndex: search: simple", "[index][search]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));

    auto result = idx.search("hello");
    REQUIRE(result.size() == 1);
}

TEST_CASE("InvertedIndex: search: multiple documents", "[index][search]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));
    idx.add(builder.build("doc2", "hello there"));
    idx.add(builder.build("doc3", "goodbye world"));

    auto result = idx.search("hello");
    std::sort(result.begin(), result.end());
    REQUIRE(result.size() == 2);
}

TEST_CASE("InvertedIndex: search: returns empty for missing word", "[index][search]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));

    REQUIRE(idx.search("foo").empty());
}

TEST_CASE("InvertedIndex: search: lowercases input before lookup", "[index][search]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "Hello World"));

    REQUIRE(idx.search("hello").size() == 1);
    REQUIRE(idx.search("Hello").size() == 1);
    REQUIRE(idx.search("HELLO").size() == 1);
}

// InvertedIndex: count

TEST_CASE("InvertedIndex: count: simple", "[index][count]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "cat cat dog cat"));

    size_t id = idx.search("cat").front();
    REQUIRE(idx.count("cat", id) == 3);
}

TEST_CASE("InvertedIndex: count: returns 0 for missing word", "[index][count]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));

    size_t id = idx.search("hello").front();
    REQUIRE(idx.count("foo", id) == 0);
}

TEST_CASE("InvertedIndex: count: returns 0 for missing doc_id", "[index][count]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));

    REQUIRE(idx.count("hello", 999) == 0);
}

// InvertedIndex: remove

TEST_CASE("InvertedIndex: remove: returns true on success", "[index][remove]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));
    REQUIRE(idx.remove("doc1"));
}

TEST_CASE("InvertedIndex: remove: returns false for missing name", "[index][remove]")
{
    InvertedIndex idx;

    REQUIRE_FALSE(idx.remove("nonexistent"));
}

// InvertedIndex: search

TEST_CASE("InvertedIndex: search: returns nothing after document is removed", "[index][remove]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));
    idx.remove("doc1");

    REQUIRE(idx.search("hello").empty());
}

TEST_CASE("InvertedIndex: word stays searchable if present in another document after remove", "[index][remove]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));
    idx.add(builder.build("doc2", "hello there"));
    idx.remove("doc1");

    REQUIRE(idx.search("hello").size() == 1);
    REQUIRE(idx.search("world").empty());
}

TEST_CASE("InvertedIndex: removed document name can be reused", "[index][remove]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));
    idx.remove("doc1");

    REQUIRE(idx.add(builder.build("doc1", "new content")));
}