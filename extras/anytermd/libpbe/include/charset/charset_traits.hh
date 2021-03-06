// charset_traits.hh
// This file is part of libpbe; see http://anyterm.org/
// (C) 2007-2008 Philip Endecott

// Distributed under the Boost Software License, Version 1.0:
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef libpbe_charset_charset_traits_hh
#define libpbe_charset_charset_traits_hh

#include "charset_t.hh"
#include "char_t.hh"


namespace pbe {

// Character Set Traits
// --------------------
//
// charset_traits<cs> provides information about a character set cs, identified
// by a member of the charset_t enumeration.  This information comprises:
// - The character type, char_t.
// - The unit type, unit_t.
// - The state type (for character sets like Shift-JIS), state_t.
// The character and unit types differ for variable-length encodings.
// Since the majority of the character sets in charset_t are fixed-length
// byte character sets with no shift-state, this is defined as a default.
// Note that different character sets' state types are distinct, even if they
// are all stateless character sets.
// - The boolean constants is_ascii_superset and is_iso8859_common_superset,
//   which indicate whether the character set shares codes 0-127 with ASCII
//   and whether it shares code 0-160 (inclusive) with the iso-8859 character
//   sets, respectively.  (The iso-8859 character sets are all ASCII supersets,
//   and they also have in common with each other characters 128 to 160
//   inclusive; 128 to 159 are control characters and 160 is non-breaking space.
// - The following functions, for conversion between the character type and
//   a sequence of units:
//   template <typename const_unit_ptr_t> char_t decode(const_unit_ptr_t& p);
//     Decodes and returns one character pointed to by p, and advances p
//     to point to the next character.
//   template <typename unit_ptr_t> void encode(unit_ptr_t& p, char_t c);
//     Encodes the character c and stores it starting at *p, and advances
//     p to point after the character that it has written.
// - The following functions, for per-character iteration though units:
//   template <typename const_unit_ptr_t> void skip_forward_char(const_unit_ptr_t& p);
//     Advance p to point to the next character.
//   template <typename const_unit_ptr_t> void skip_backward_char(const_unit_ptr_t& p);
//     Move p back to point to the previous character.
//   template <typename const_unit_ptr_t> int char_length(const_unit_ptr_t& p);
//     Returns the length in units of the character pointed to by p.
// - The following to help client code allocate buffer space:
//   size_t max_characters(size_t n_units)
//   size_t typ_characters(size_t n_units)
//   size_t max_units(size_t n_characters)
//   size_t typ_units(size_t n_characters)

// Maybe add: check whether units pointed to are valid.

// For fixed-length character sets, most of these things are trivial; the
// only variable is whether they have the common ASCII or ISO-8859 subset.
// Specialisations for such cases can inherit from one of these:

// cset must be a template parameter so that the state_t for each character
// set is distinct.
template <charset_t cset, typename C>
struct fixed_charset_traits {
  typedef C char_t;
  typedef C unit_t;
  struct state_t {};
  static const bool is_ascii_superset = false;
  static const bool is_ascii_plus_c1_superset = false;
  template <typename const_unit_ptr_t> static char_t decode(const_unit_ptr_t& p) { return *(p++); }
  template <typename unit_ptr_t>       static void encode(unit_ptr_t& p, char_t c) { *(p++) = c; }
  template <typename const_unit_ptr_t> static void skip_forward_char(const_unit_ptr_t& p) { ++p; }
  template <typename const_unit_ptr_t> static void skip_backward_char(const_unit_ptr_t& p) { --p; }
  template <typename const_unit_ptr_t> static int char_length(const_unit_ptr_t& p) { return 1; }
  static size_t max_characters(size_t n_units) { return n_units; }
  static size_t typ_characters(size_t n_units) { return n_units; }
  static size_t max_units(size_t n_characters) { return n_characters; }
  static size_t typ_units(size_t n_characters) { return n_characters; }
};

template <charset_t cset>
struct fixed_byte_charset_traits: fixed_charset_traits<cset,char8_t> {};

template <charset_t cset>
struct fixed_byte_ascii_superset_charset_traits: fixed_byte_charset_traits<cset> {
  static const bool is_ascii_superset = true;
  static const bool is_ascii_plus_c1_superset = false;
};

template <charset_t cset>
struct fixed_byte_iso8859_common_superset_charset_traits: fixed_byte_charset_traits<cset> {
  static const bool is_ascii_superset = true;
  static const bool is_ascii_plus_c1_superset = true;
};


// Here is the base charset traits class, for which specialisations are made:

template <charset_t cset>
struct charset_traits {};


// For many of the fixed-length character sets, we define specialisations here:

template <> struct charset_traits<cs::ascii>:      fixed_byte_ascii_superset_charset_traits<cs::ascii> {};

// The ISO-8859 character sets:
template <> struct charset_traits<cs::iso8859_1>:  fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_1> {};
template <> struct charset_traits<cs::iso8859_2>:  fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_2> {};
template <> struct charset_traits<cs::iso8859_3>:  fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_3> {};
template <> struct charset_traits<cs::iso8859_4>:  fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_4> {};
template <> struct charset_traits<cs::iso8859_5>:  fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_5> {};
template <> struct charset_traits<cs::iso8859_6>:  fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_6> {};
template <> struct charset_traits<cs::iso8859_7>:  fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_7> {};
template <> struct charset_traits<cs::iso8859_8>:  fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_8> {};
template <> struct charset_traits<cs::iso8859_9>:  fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_9> {};
template <> struct charset_traits<cs::iso8859_10>: fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_10> {};
template <> struct charset_traits<cs::iso8859_13>: fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_13> {};
template <> struct charset_traits<cs::iso8859_14>: fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_14> {};
template <> struct charset_traits<cs::iso8859_15>: fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_15> {};
template <> struct charset_traits<cs::iso8859_16>: fixed_byte_iso8859_common_superset_charset_traits<cs::iso8859_16> {};
// These sound related, but I don't know what they are:
// iso8859_6_e
// iso8859_6_i
// iso8859_supp

// The ISO-646 character sets.  These are 7-bit character sets with most characters shared
// with ASCII, but with some punctuation characters replaced by national accented characters
// and symbols.
// It might be useful to have a trait indicating that they're "approximately" ASCII.
template <> struct charset_traits<cs::iso646_gb>:  fixed_byte_charset_traits<cs::iso646_gb> {};
template <> struct charset_traits<cs::iso646_se2>: fixed_byte_charset_traits<cs::iso646_se2> {};
template <> struct charset_traits<cs::iso646_it>:  fixed_byte_charset_traits<cs::iso646_it> {};
template <> struct charset_traits<cs::iso646_es>:  fixed_byte_charset_traits<cs::iso646_es> {};
template <> struct charset_traits<cs::iso646_de>:  fixed_byte_charset_traits<cs::iso646_de> {};
template <> struct charset_traits<cs::iso646_no>:  fixed_byte_charset_traits<cs::iso646_no> {};
template <> struct charset_traits<cs::iso646_fr>:  fixed_byte_charset_traits<cs::iso646_fr> {};
template <> struct charset_traits<cs::iso646_se>:  fixed_byte_charset_traits<cs::iso646_se> {};
template <> struct charset_traits<cs::iso646_pt>:  fixed_byte_charset_traits<cs::iso646_pt> {};
template <> struct charset_traits<cs::iso646_fr1>: fixed_byte_charset_traits<cs::iso646_fr1> {};
template <> struct charset_traits<cs::iso646_no2>: fixed_byte_charset_traits<cs::iso646_no2> {};
template <> struct charset_traits<cs::iso646_pt2>: fixed_byte_charset_traits<cs::iso646_pt2> {};
template <> struct charset_traits<cs::iso646_es2>: fixed_byte_charset_traits<cs::iso646_es2> {};
template <> struct charset_traits<cs::iso646_hu>:  fixed_byte_charset_traits<cs::iso646_hu> {};
template <> struct charset_traits<cs::iso646_jp_ocr_b>:  fixed_byte_charset_traits<cs::iso646_jp_ocr_b> {};
template <> struct charset_traits<cs::iso646_ca>:  fixed_byte_charset_traits<cs::iso646_ca> {};
template <> struct charset_traits<cs::iso646_ca2>: fixed_byte_charset_traits<cs::iso646_ca2> {};
template <> struct charset_traits<cs::iso646_yu>:  fixed_byte_charset_traits<cs::iso646_yu> {};
template <> struct charset_traits<cs::iso646_cu>:  fixed_byte_charset_traits<cs::iso646_cu> {};
template <> struct charset_traits<cs::iso646_dk>:  fixed_byte_charset_traits<cs::iso646_dk> {};
template <> struct charset_traits<cs::iso646_kr>:  fixed_byte_charset_traits<cs::iso646_kr> {};
// This sounds related, but I don't know what it is:
// ISO_646.basic:1983

// These "code pages" are all 8-bit fixed-length encodings.
// Some/many of them will be ASCII supersets but this isn't identified here.
// (In some/most cases a definition is provided in the Unicode mapping tables.)
template <> struct charset_traits<cs::cp037>:  fixed_byte_charset_traits<cs::cp037> {};
template <> struct charset_traits<cs::cp038>:  fixed_byte_charset_traits<cs::cp038> {};
template <> struct charset_traits<cs::cp154>:  fixed_byte_charset_traits<cs::cp154> {};
template <> struct charset_traits<cs::cp273>:  fixed_byte_charset_traits<cs::cp273> {};
template <> struct charset_traits<cs::cp274>:  fixed_byte_charset_traits<cs::cp274> {};
template <> struct charset_traits<cs::cp275>:  fixed_byte_charset_traits<cs::cp275> {};
template <> struct charset_traits<cs::ibm277>: fixed_byte_charset_traits<cs::ibm277> {};
template <> struct charset_traits<cs::cp278>:  fixed_byte_charset_traits<cs::cp278> {};
template <> struct charset_traits<cs::cp280>:  fixed_byte_charset_traits<cs::cp280> {};
template <> struct charset_traits<cs::cp281>:  fixed_byte_charset_traits<cs::cp281> {};
template <> struct charset_traits<cs::cp284>:  fixed_byte_charset_traits<cs::cp284> {};
template <> struct charset_traits<cs::cp285>:  fixed_byte_charset_traits<cs::cp285> {};
template <> struct charset_traits<cs::cp290>:  fixed_byte_charset_traits<cs::cp290> {};
template <> struct charset_traits<cs::cp297>:  fixed_byte_charset_traits<cs::cp297> {};
template <> struct charset_traits<cs::cp420>:  fixed_byte_charset_traits<cs::cp420> {};
template <> struct charset_traits<cs::cp423>:  fixed_byte_charset_traits<cs::cp423> {};
template <> struct charset_traits<cs::cp424>:  fixed_byte_charset_traits<cs::cp424> {};
template <> struct charset_traits<cs::cp437>:  fixed_byte_charset_traits<cs::cp437> {};
template <> struct charset_traits<cs::cp500>:  fixed_byte_charset_traits<cs::cp500> {};
template <> struct charset_traits<cs::cp775>:  fixed_byte_charset_traits<cs::cp775> {};
template <> struct charset_traits<cs::cp850>:  fixed_byte_charset_traits<cs::cp850> {};
template <> struct charset_traits<cs::cp851>:  fixed_byte_charset_traits<cs::cp851> {};
template <> struct charset_traits<cs::cp852>:  fixed_byte_charset_traits<cs::cp852> {};
template <> struct charset_traits<cs::cp855>:  fixed_byte_charset_traits<cs::cp855> {};
template <> struct charset_traits<cs::cp857>:  fixed_byte_charset_traits<cs::cp857> {};
template <> struct charset_traits<cs::cp860>:  fixed_byte_charset_traits<cs::cp860> {};
template <> struct charset_traits<cs::cp861>:  fixed_byte_charset_traits<cs::cp861> {};
template <> struct charset_traits<cs::cp862>:  fixed_byte_charset_traits<cs::cp862> {};
template <> struct charset_traits<cs::cp863>:  fixed_byte_charset_traits<cs::cp863> {};
template <> struct charset_traits<cs::cp864>:  fixed_byte_charset_traits<cs::cp864> {};
template <> struct charset_traits<cs::cp865>:  fixed_byte_charset_traits<cs::cp865> {};
template <> struct charset_traits<cs::cp866>:  fixed_byte_charset_traits<cs::cp866> {};
template <> struct charset_traits<cs::cp868>:  fixed_byte_charset_traits<cs::cp868> {};
template <> struct charset_traits<cs::cp869>:  fixed_byte_charset_traits<cs::cp869> {};
template <> struct charset_traits<cs::cp870>:  fixed_byte_charset_traits<cs::cp870> {};
template <> struct charset_traits<cs::cp871>:  fixed_byte_charset_traits<cs::cp871> {};
template <> struct charset_traits<cs::cp880>:  fixed_byte_charset_traits<cs::cp880> {};
template <> struct charset_traits<cs::cp891>:  fixed_byte_charset_traits<cs::cp891> {};
template <> struct charset_traits<cs::cp903>:  fixed_byte_charset_traits<cs::cp903> {};
template <> struct charset_traits<cs::cp904>:  fixed_byte_charset_traits<cs::cp904> {};
template <> struct charset_traits<cs::cp905>:  fixed_byte_charset_traits<cs::cp905> {};
template <> struct charset_traits<cs::cp918>:  fixed_byte_charset_traits<cs::cp918> {};
template <> struct charset_traits<cs::windows936>: fixed_byte_charset_traits<cs::windows936> {};
template <> struct charset_traits<cs::cp1026>: fixed_byte_charset_traits<cs::cp1026> {};
template <> struct charset_traits<cs::ibm1047>:  fixed_byte_charset_traits<cs::ibm1047> {};
template <> struct charset_traits<cs::windows_1250>:  fixed_byte_charset_traits<cs::windows_1250> {};
template <> struct charset_traits<cs::windows_1251>:  fixed_byte_charset_traits<cs::windows_1251> {};
template <> struct charset_traits<cs::windows_1252>:  fixed_byte_charset_traits<cs::windows_1252> {};
template <> struct charset_traits<cs::windows_1253>:  fixed_byte_charset_traits<cs::windows_1253> {};
template <> struct charset_traits<cs::windows_1254>:  fixed_byte_charset_traits<cs::windows_1254> {};
template <> struct charset_traits<cs::windows_1255>:  fixed_byte_charset_traits<cs::windows_1255> {};
template <> struct charset_traits<cs::windows_1256>:  fixed_byte_charset_traits<cs::windows_1256> {};
template <> struct charset_traits<cs::windows_1257>:  fixed_byte_charset_traits<cs::windows_1257> {};
template <> struct charset_traits<cs::windows_1258>:  fixed_byte_charset_traits<cs::windows_1258> {};
template <> struct charset_traits<cs::cp00858>:  fixed_byte_charset_traits<cs::cp00858> {};
template <> struct charset_traits<cs::cp00924>:  fixed_byte_charset_traits<cs::cp00924> {};
template <> struct charset_traits<cs::cp01140>:  fixed_byte_charset_traits<cs::cp01140> {};
template <> struct charset_traits<cs::cp01141>:  fixed_byte_charset_traits<cs::cp01141> {};
template <> struct charset_traits<cs::cp01142>:  fixed_byte_charset_traits<cs::cp01142> {};
template <> struct charset_traits<cs::cp01143>:  fixed_byte_charset_traits<cs::cp01143> {};
template <> struct charset_traits<cs::cp01144>:  fixed_byte_charset_traits<cs::cp01144> {};
template <> struct charset_traits<cs::cp01145>:  fixed_byte_charset_traits<cs::cp01145> {};
template <> struct charset_traits<cs::cp01146>:  fixed_byte_charset_traits<cs::cp01146> {};
template <> struct charset_traits<cs::cp01147>:  fixed_byte_charset_traits<cs::cp01147> {};
template <> struct charset_traits<cs::cp01148>:  fixed_byte_charset_traits<cs::cp01148> {};
template <> struct charset_traits<cs::cp01149>:  fixed_byte_charset_traits<cs::cp01149> {};

// The cyrillic KOI8 character sets are ASCII supersets.
template <> struct charset_traits<cs::koi8_e>:  fixed_byte_ascii_superset_charset_traits<cs::koi8_e> {};
template <> struct charset_traits<cs::koi8_r>:  fixed_byte_ascii_superset_charset_traits<cs::koi8_r> {};
template <> struct charset_traits<cs::koi8_u>:  fixed_byte_ascii_superset_charset_traits<cs::koi8_u> {};


// The fixed Unicode formats:
template <> struct charset_traits<cs::ucs2>:  fixed_charset_traits<cs::ucs2,char16_t> {};
template <> struct charset_traits<cs::ucs4>:  fixed_charset_traits<cs::ucs4,char32_t> {};


// For the variable-length, specialisations are
// provided in separate files.  These are:
// utf8
// utf16
// iso_10646_utf_1
// unicode_1_1_utf_7
// UTF-7
// UTF16-BE
// UTF16-LE
// UTF32
// UTF32-BE
// UTF32-LE
// iso_2022_kr
// iso_2022_jp
// iso_2022_jp_2
// iso_2022_cn
// iso_2022_cn_ext
// shift_jis


// I don't yet know enough about the following character sets to supply traits
// for them.  I do not aim to do so for every single character set.

// iso_ir_142
// jis_x0201
// jis_encoding
// euc_jp
// Extended_UNIX_Code_Fixed_Width_for_Japanese
// INVARIANT
// iso_ir_2
// iso_ir_8_1
// iso_ir_8_2
// iso_ir_9_1
// iso_ir_9_2
// iso_ir_149
// euc_kr
// iso_ir_13
// iso_ir_14
// iso_ir_18
// iso_ir_19
// iso_ir_27
// iso_ir_37
// iso_ir_42
// iso_ir_47
// iso_ir_49
// iso_ir_50
// iso_ir_51
// iso_ir_52
// iso_ir_53
// iso_ir_54
// iso_ir_55
// iso_ir_57
// iso_ir_58
// iso_ir_70
// iso_ir_87
// iso_ir_88
// iso_ir_89
// iso_ir_90
// iso_ir_91
// iso_ir_93
// iso_ir_94
// iso_ir_95
// iso_ir_96
// iso_ir_98
// iso_ir_99
// iso_ir_102
// iso_ir_103
// iso_ir_103
// iso_ir_123
// iso_ir_139
// iso_ir_143
// iso_ir_146
// iso_ir_147
// iso_ir_150
// iso_ir_152
// iso_ir_153
// iso_ir_155
// iso_ir_158
// iso_ir_159
// us-dk
// dk-us
// gb18030
// OSD_EBCDIC_DF04_15
// OSD_EBCDIC_DF03_IRV
// OSD_EBCDIC_DF04_1
// ISO-11548-1
// KZ-1048
// ISO-10646-UCS-Basic
// ISO-10646-Unicode-Latin1
// ISO-10646-J-1
// ISO-Unicode-IBM-1261
// ISO-Unicode-IBM-1268
// ISO-Unicode-IBM-1276
// ISO-Unicode-IBM-1264
// ISO-Unicode-IBM-1265
// UNICODE-1-1
// SCSU
// CESU-8
// BOCU-1
// ISO-8859-1-Windows-3.0-Latin-1
// ISO-8859-1-Windows-3.1-Latin-1
// ISO-8859-2-Windows-Latin-2
// ISO-8859-9-Windows-Latin-5
// hp_roman8
// Adobe-Standard-Encoding
// Ventura-US
// Ventura-International
// DEC-MCS
// PC8-Danish-Norwegian
// PC8-Turkish
// IBM-Symbols
// IBM-Thai
// HP-Legal
// HP-Pi-font
// HP-Math8
// Adobe-Symbol-Encoding
// HP-DeskTop
// Ventura-Math
// Microsoft-Publishing
// Windows-31J
// GB2312
// Big5
// macintosh
// EBCDIC-AT-DE
// EBCDIC-AT-DE-A
// EBCDIC-CA-FR
// EBCDIC-DK-NO
// EBCDIC-DK-NO-A
// EBCDIC-FI-SE
// EBCDIC-FI-SE-A
// EBCDIC-FR
// EBCDIC-IT
// EBCDIC-PT
// EBCDIC-ES
// EBCDIC-ES-A
// EBCDIC-ES-S
// EBCDIC-UK
// EBCDIC-US
// UNKNOWN-8BIT
// MNEMONIC
// MNEM
// VISCII
// VIQR
// HZ-GB-2312
// Big5-HKSCS
// Amiga1251
// KOI7-switched
// BRF
// TSCII
// TIS-620



};



#endif
