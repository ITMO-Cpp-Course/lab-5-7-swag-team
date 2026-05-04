#include <catch2/catch_all.hpp>

#include "document_builder.hpp"
#include "index_store.hpp"
#include "inverted_index.hpp"
#include "update_transaction.hpp"

#include <algorithm>

using namespace lab5::index;

// DocumentBuilder

TEST_CASE("DocumentBuilder: basic tokenization", "[builder]")
{
    DocumentBuilder builder;
    Document doc = builder.build("doc", "Hello World");

    REQUIRE(doc.name == "doc");
    REQUIRE(doc.id == 0);
    REQUIRE(doc.words == std::vector<std::string>{"hello", "world"});
}

TEST_CASE("DocumentBuilder: punctuation is stripped", "[builder]")
{
    DocumentBuilder builder;
    Document doc = builder.build("doc", "Hello&!(#%@ World");

    REQUIRE(doc.words == std::vector<std::string>{"hello", "world"});
}

TEST_CASE("DocumentBuilder: lowercase", "[builder]")
{
    DocumentBuilder builder;
    Document doc = builder.build("doc", "FOO BAR Baz"); // baz lighter

    REQUIRE(doc.words == std::vector<std::string>{"foo", "bar", "baz"}); // baz lighter
}

TEST_CASE("DocumentBuilder: hyphenated word is preserved", "[builder]")
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

// InvertedIndex

// InvertedIndex: add

TEST_CASE("InvertedIndex: add returns true on success", "[index][add]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    REQUIRE(idx.add(builder.build("doc1", "hello world")).has_value());
}

TEST_CASE("InvertedIndex: add rejects duplicate name", "[index][add]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));
    REQUIRE_FALSE(idx.add(builder.build("doc1", "other text")).has_value());
}

TEST_CASE("InvertedIndex: add rejects empty name", "[index][add]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    REQUIRE_FALSE(idx.add(builder.build("", "hello world")).has_value());
}

TEST_CASE("InvertedIndex: add rejects document with no words", "[index][add]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    REQUIRE_FALSE(idx.add(builder.build("doc1", "")).has_value());
}

// InvertedIndex: search

TEST_CASE("InvertedIndex: search finds word in single document", "[index][search]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));

    auto result = idx.search("hello");
    REQUIRE(result.size() == 1);
}

TEST_CASE("InvertedIndex: search finds word across multiple documents", "[index][search]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));
    idx.add(builder.build("doc2", "hello there"));
    idx.add(builder.build("doc3", "goodbye world"));

    auto result = idx.search("hello");
    REQUIRE(result.size() == 2);
    REQUIRE(std::find(result.begin(), result.end(), "doc1") != result.end());
    REQUIRE(std::find(result.begin(), result.end(), "doc2") != result.end());
    REQUIRE(std::find(result.begin(), result.end(), "doc3") == result.end());
}

TEST_CASE("InvertedIndex: search: hyphenated word is found", "[index][search]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "well-known fact"));

    REQUIRE(idx.search("well-known").size() == 1);
    REQUIRE(idx.search("well").empty());
}

TEST_CASE("InvertedIndex: search: returns empty for missing word", "[index][search]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));

    REQUIRE(idx.search("foo").empty());
}

TEST_CASE("InvertedIndex: search lowercases input before lookup", "[index][search]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "Hello World"));

    REQUIRE(idx.search("hello").size() == 1);
    REQUIRE(idx.search("Hello").size() == 1);
    REQUIRE(idx.search("HELLO").size() == 1);
}

// InvertedIndex: count

TEST_CASE("InvertedIndex: count returns correct occurrence count", "[index][count]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "cat cat dog cat"));

    REQUIRE(idx.count("cat", "doc1").value() == 3);
}

TEST_CASE("InvertedIndex: count: lowercases input before lookup", "[index][count]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "cat cat dog cat"));

    REQUIRE(idx.count("CAT", "doc1").value() == 3);
    REQUIRE(idx.count("Cat", "doc1").value() == 3);
}

TEST_CASE("InvertedIndex: count returns 0 for missing word", "[index][count]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));

    REQUIRE(idx.count("foo", "doc1").value() == 0);
}

TEST_CASE("InvertedIndex: count: returns error for missing doc name", "[index][count]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));

    REQUIRE_FALSE(idx.count("hello", "nonexistent").has_value());
}

// InvertedIndex: remove

TEST_CASE("InvertedIndex: remove returns true on success", "[index][remove]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));
    REQUIRE(idx.remove("doc1").has_value());
}

TEST_CASE("InvertedIndex: remove returns false for missing name", "[index][remove]")
{
    InvertedIndex idx;

    REQUIRE_FALSE(idx.remove("nonexistent").has_value());
}

TEST_CASE("InvertedIndex: search returns nothing after document is removed", "[index][remove]")
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

TEST_CASE("InvertedIndex: count: returns 0 after document is removed", "[index][remove]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));
    idx.remove("doc1");

    REQUIRE_FALSE(idx.count("hello", "doc1").has_value());
}

TEST_CASE("InvertedIndex: removed document name can be reused", "[index][remove]")
{
    InvertedIndex idx;
    DocumentBuilder builder;

    idx.add(builder.build("doc1", "hello world"));
    idx.remove("doc1");

    REQUIRE(idx.add(builder.build("doc1", "new content")).has_value());
}

// IndexStore

// IndexStore: errors

TEST_CASE("IndexStore: add returns error for duplicate name", "[store][error]")
{
    IndexStore store;
    DocumentBuilder builder;

    store.add(builder.build("doc1", "hello world"));
    REQUIRE_FALSE(store.add(builder.build("doc1", "other text")).has_value());
}

TEST_CASE("IndexStore: add returns error for empty name", "[store][error]")
{
    IndexStore store;
    DocumentBuilder builder;

    REQUIRE_FALSE(store.add(builder.build("", "hello world")).has_value());
}

TEST_CASE("IndexStore: add returns error for empty text", "[store][error]")
{
    IndexStore store;
    DocumentBuilder builder;

    REQUIRE_FALSE(store.add(builder.build("doc1", "")).has_value());
}

TEST_CASE("IndexStore: remove returns error for missing document", "[store][error]")
{
    IndexStore store;

    REQUIRE_FALSE(store.remove("nonexistent").has_value());
}

TEST_CASE("IndexStore: count returns error for missing document", "[store][error]")
{
    IndexStore store;
    DocumentBuilder builder;

    store.add(builder.build("doc1", "hello world"));

    REQUIRE_FALSE(store.count("hello", "nonexistent").has_value());
}

// IndexStore: UpdateTransaction

TEST_CASE("UpdateTransaction: commit applies changes", "[store][transaction]")
{
    IndexStore store;
    DocumentBuilder builder;

    auto tx = store.start_transaction();
    tx.add(builder.build("doc1", "hello world"));
    tx.commit();

    REQUIRE(store.search("hello").value().size() == 1);
}

TEST_CASE("UpdateTransaction: rollback on destruction without commit", "[store][transaction]")
{
    IndexStore store;
    DocumentBuilder builder;

    {
        auto tx = store.start_transaction();
        tx.add(builder.build("doc1", "hello world"));
    }

    REQUIRE(store.search("hello").value().empty());
}

TEST_CASE("UpdateTransaction: rollback restores removed document", "[store][transaction]")
{
    IndexStore store;
    DocumentBuilder builder;

    store.add(builder.build("doc1", "hello world"));

    {
        auto tx = store.start_transaction();
        tx.remove("doc1");
    }

    REQUIRE(store.search("hello").value().size() == 1);
}

TEST_CASE("UpdateTransaction: partial rollback on failed add", "[store][transaction]")
{
    IndexStore store;
    DocumentBuilder builder;

    store.add(builder.build("doc1", "hello world"));

    {
        auto tx = store.start_transaction();
        tx.add(builder.build("doc2", "hello there"));
        REQUIRE_FALSE(tx.add(builder.build("doc1", "duplicate")).has_value());
    }

    auto result = store.search("hello").value();
    REQUIRE(result.size() == 1);
    REQUIRE(std::find(result.begin(), result.end(), "doc1") != result.end());
    REQUIRE(std::find(result.begin(), result.end(), "doc2") == result.end());
}