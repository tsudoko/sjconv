sjconv
======

**sjconv** is an implementation of the POSIX `iconv(3)` API. Right now it only
supports converting text from Windows code page 932 (a non-standard extension of
Shift_JIS) to UTF-8. Support for other encodings is not planned. Conversion from
UTF-8 to cp932 might be supported in the future.
