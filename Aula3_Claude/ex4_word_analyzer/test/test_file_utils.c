/* Unity tests for the file_utils module (word analyzer) */
#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "file_utils.h"
#include "word_list.h"

static TextStats stats;
static WordList *words;

/* Every test starts with a new, empty word list */
void setUp(void) {
    words = word_list_create();
    TEST_ASSERT_NOT_NULL(words);
}

void tearDown(void) {
    word_list_destroy(words);
    words = NULL;
}

/* ---------------------------------------------------------------------- */
/* Helpers                                                                 */
/* ---------------------------------------------------------------------- */

/* Analyzes text and checks the three counters */
static void analyze(const char *text, unsigned long lines,
                    unsigned long nwords, unsigned long chars) {
    TEST_ASSERT_EQUAL_INT(FILE_UTILS_OK, file_utils_analyze_string(text, &stats, words));
    TEST_ASSERT_EQUAL_UINT_MESSAGE(lines, stats.lines, "lines");
    TEST_ASSERT_EQUAL_UINT_MESSAGE(nwords, stats.words, "words");
    TEST_ASSERT_EQUAL_UINT_MESSAGE(chars, stats.characters, "characters");
    /* every word that was counted must also be in the list */
    TEST_ASSERT_EQUAL_UINT_MESSAGE(nwords, word_list_total(words), "words in the list");
}

static void assert_frequency(unsigned long expected, const char *word) {
    TEST_ASSERT_EQUAL_UINT_MESSAGE(expected, word_list_frequency(words, word), word);
}

/* tmpfile() is standard C, but with MinGW it needs administrator rights (it
 * creates the file in C:\), so in that case a file in build/ is used */
#define SCRATCH_NAME "build/test_file_utils.tmp"

static FILE *scratch_open(void) {
    FILE *f = tmpfile();
    return f != NULL ? f : fopen(SCRATCH_NAME, "w+b");
}

static void scratch_close(FILE *f) {
    fclose(f);
    remove(SCRATCH_NAME);       /* nothing to remove if tmpfile() worked */
}

/* ---------------------------------------------------------------------- */
/* Lines, words and characters                                             */
/* ---------------------------------------------------------------------- */

void test_EmptyText(void) {
    analyze("", 0, 0, 0);
    TEST_ASSERT_EQUAL_size_t(0, word_list_size(words));
}

void test_OneLine(void) {
    analyze("hello world\n", 1, 2, 12);
    assert_frequency(1, "hello");
    assert_frequency(1, "world");
}

void test_LastLineWithoutNewline(void) {
    analyze("one\ntwo", 2, 2, 7);
}

void test_EmptyLinesAreLines(void) {
    analyze("\n\n\n", 3, 0, 3);
}

void test_TabsSpacesAndWindowsLineEndings(void) {
    analyze("a\tb  c\r\nd\r\n", 2, 4, 11);
}

/* ---------------------------------------------------------------------- */
/* What is a word                                                          */
/* ---------------------------------------------------------------------- */

void test_UpperAndLowerCaseAreTheSameWord(void) {
    analyze("The the THE tHe", 1, 4, 15);
    assert_frequency(4, "the");
    TEST_ASSERT_EQUAL_size_t(1, word_list_size(words));
}

void test_PunctuationSeparatesWords(void) {
    const char *text = "Hello, world! Hello... world? (hello)";

    analyze(text, 1, 5, (unsigned long)strlen(text));
    assert_frequency(3, "hello");
    assert_frequency(2, "world");
}

void test_ApostrophesAndHyphensInsideWords(void) {
    const char *text = "Don't stop: real-time, well-known 'quoted' -dash- rock--roll";

    analyze(text, 1, 8, (unsigned long)strlen(text));
    assert_frequency(1, "don't");
    assert_frequency(1, "stop");
    assert_frequency(1, "real-time");
    assert_frequency(1, "well-known");
    assert_frequency(1, "quoted");
    assert_frequency(1, "dash");
    assert_frequency(1, "rock");
    assert_frequency(1, "roll");
}

void test_NumbersAreWords(void) {
    const char *text = "SETR 2026: lab 3, lab 4";

    analyze(text, 1, 6, (unsigned long)strlen(text));
    assert_frequency(2, "lab");
    assert_frequency(1, "2026");
}

/* ---------------------------------------------------------------------- */
/* Accents and other encodings                                             */
/* ---------------------------------------------------------------------- */

void test_AccentedLettersInUtf8(void) {
    /* 15 characters in 21 bytes: each accented letter takes 2 bytes */
    analyze("Ação ação AÇÃO\n", 1, 3, 15);
    assert_frequency(3, "ação");
    TEST_ASSERT_EQUAL_size_t(1, word_list_size(words));
}

void test_TypographicPunctuationSeparatesWords(void) {
    analyze("“Olá” — disse ele, ‘não’…", 1, 4, 25);
    assert_frequency(1, "olá");
    assert_frequency(1, "disse");
    assert_frequency(1, "ele");
    assert_frequency(1, "não");
}

void test_TypographicApostropheIsAnApostrophe(void) {
    analyze("don’t don't DON'T", 1, 3, 17);
    assert_frequency(3, "don't");
}

void test_Latin1TextIsAccepted(void) {
    /* "ação AÇÃO\n" saved in Latin-1 (one byte per letter): not valid UTF-8 */
    analyze("a\xE7\xE3o A\xC7\xC3O\n", 1, 2, 10);
    assert_frequency(2, "ação");
}

void test_ByteOrderMarkIsNotPartOfTheWord(void) {
    analyze("\xEF\xBB\xBF" "hello\n", 1, 1, 7);
    assert_frequency(1, "hello");
}

void test_SymbolsAndEmojiSeparateWords(void) {
    analyze("price: 5€ 😀ok", 1, 3, 13);
    assert_frequency(1, "price");
    assert_frequency(1, "5");
    assert_frequency(1, "ok");
}

/* ---------------------------------------------------------------------- */
/* Long words                                                              */
/* ---------------------------------------------------------------------- */

void test_LongWordIsCountedAndTruncated(void) {
    char text[FILE_UTILS_MAX_WORD + 40];
    char expected[FILE_UTILS_MAX_WORD + 1];

    memset(text, 'a', FILE_UTILS_MAX_WORD + 30);        /* one 94-letter word */
    strcpy(text + FILE_UTILS_MAX_WORD + 30, " b");
    memset(expected, 'a', FILE_UTILS_MAX_WORD);
    expected[FILE_UTILS_MAX_WORD] = '\0';

    analyze(text, 1, 2, FILE_UTILS_MAX_WORD + 32);
    assert_frequency(1, expected);
    assert_frequency(1, "b");
}

void test_TruncationNeverSplitsACharacter(void) {
    /* 63 letters + "ç" (2 bytes) do not fit in 64 bytes: the stored word is
     * just the 63 letters (the "y" after the "ç" is dropped too) */
    char text[FILE_UTILS_MAX_WORD + 8];
    char expected[FILE_UTILS_MAX_WORD];

    memset(text, 'x', FILE_UTILS_MAX_WORD - 1);
    strcpy(text + FILE_UTILS_MAX_WORD - 1, "çy");
    memset(expected, 'x', FILE_UTILS_MAX_WORD - 1);
    expected[FILE_UTILS_MAX_WORD - 1] = '\0';

    analyze(text, 1, 1, FILE_UTILS_MAX_WORD + 1);
    assert_frequency(1, expected);
}

/* ---------------------------------------------------------------------- */
/* Streams, files and errors                                               */
/* ---------------------------------------------------------------------- */

void test_WithoutWordListOnlyCounts(void) {
    TEST_ASSERT_EQUAL_INT(FILE_UTILS_OK, file_utils_analyze_string("a b\nc", &stats, NULL));
    TEST_ASSERT_EQUAL_UINT(2, stats.lines);
    TEST_ASSERT_EQUAL_UINT(3, stats.words);
    TEST_ASSERT_EQUAL_UINT(5, stats.characters);
}

void test_AnalyzeStream(void) {
    FILE *f = scratch_open();
    int rc;

    if (f == NULL) {
        TEST_IGNORE_MESSAGE("cannot create a temporary file");
        return;
    }
    fputs("Real-time systems\nreal time\n", f);
    rewind(f);
    rc = file_utils_analyze_stream(f, &stats, words);
    scratch_close(f);

    TEST_ASSERT_EQUAL_INT(FILE_UTILS_OK, rc);
    TEST_ASSERT_EQUAL_UINT(2, stats.lines);
    TEST_ASSERT_EQUAL_UINT(4, stats.words);
    TEST_ASSERT_EQUAL_UINT(28, stats.characters);
    assert_frequency(1, "real-time");
    assert_frequency(1, "real");
    assert_frequency(1, "time");
    assert_frequency(1, "systems");
}

void test_CharacterSplitBetweenTwoReads(void) {
    /* The file is read in blocks of 4096 bytes: bytes 4095 and 4096 hold the
     * two bytes of "ç", so the decoder must keep its state between blocks */
    FILE *f = scratch_open();
    int rc;

    if (f == NULL) {
        TEST_IGNORE_MESSAGE("cannot create a temporary file");
        return;
    }
    for (int i = 0; i < 4095; i++) {
        fputc('a', f);
    }
    fputs("ç\n", f);
    rewind(f);
    rc = file_utils_analyze_stream(f, &stats, words);
    scratch_close(f);

    TEST_ASSERT_EQUAL_INT(FILE_UTILS_OK, rc);
    TEST_ASSERT_EQUAL_UINT(1, stats.lines);
    TEST_ASSERT_EQUAL_UINT(1, stats.words);
    TEST_ASSERT_EQUAL_UINT(4097, stats.characters);
}

void test_MissingFile(void) {
    TEST_ASSERT_EQUAL_INT(FILE_UTILS_ERR_OPEN,
                          file_utils_analyze_file("no/such/dir/file.txt", &stats, words));
}

void test_InvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(FILE_UTILS_ERR_ARG, file_utils_analyze_string(NULL, &stats, words));
    TEST_ASSERT_EQUAL_INT(FILE_UTILS_ERR_ARG, file_utils_analyze_string("x", NULL, words));
    TEST_ASSERT_EQUAL_INT(FILE_UTILS_ERR_ARG, file_utils_analyze_stream(NULL, &stats, words));
    TEST_ASSERT_EQUAL_INT(FILE_UTILS_ERR_ARG, file_utils_analyze_file(NULL, &stats, words));
}

void test_ErrorMessages(void) {
    TEST_ASSERT_EQUAL_STRING("success", file_utils_strerror(FILE_UTILS_OK));
    TEST_ASSERT_EQUAL_STRING("cannot open file", file_utils_strerror(FILE_UTILS_ERR_OPEN));
    TEST_ASSERT_EQUAL_STRING("unknown error", file_utils_strerror(12345));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_EmptyText);
    RUN_TEST(test_OneLine);
    RUN_TEST(test_LastLineWithoutNewline);
    RUN_TEST(test_EmptyLinesAreLines);
    RUN_TEST(test_TabsSpacesAndWindowsLineEndings);

    RUN_TEST(test_UpperAndLowerCaseAreTheSameWord);
    RUN_TEST(test_PunctuationSeparatesWords);
    RUN_TEST(test_ApostrophesAndHyphensInsideWords);
    RUN_TEST(test_NumbersAreWords);

    RUN_TEST(test_AccentedLettersInUtf8);
    RUN_TEST(test_TypographicPunctuationSeparatesWords);
    RUN_TEST(test_TypographicApostropheIsAnApostrophe);
    RUN_TEST(test_Latin1TextIsAccepted);
    RUN_TEST(test_ByteOrderMarkIsNotPartOfTheWord);
    RUN_TEST(test_SymbolsAndEmojiSeparateWords);

    RUN_TEST(test_LongWordIsCountedAndTruncated);
    RUN_TEST(test_TruncationNeverSplitsACharacter);

    RUN_TEST(test_WithoutWordListOnlyCounts);
    RUN_TEST(test_AnalyzeStream);
    RUN_TEST(test_CharacterSplitBetweenTwoReads);
    RUN_TEST(test_MissingFile);
    RUN_TEST(test_InvalidArguments);
    RUN_TEST(test_ErrorMessages);

    return UNITY_END();
}
