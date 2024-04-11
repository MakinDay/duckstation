// SPDX-FileCopyrightText: 2019-2024 Connor McLaughlin <stenzek@gmail.com>
// SPDX-License-Identifier: (GPL-3.0 OR CC-BY-NC-ND-4.0)

#pragma once

#include "small_string.h"
#include "types.h"

#include "fmt/core.h"

#include <string>
#include <string_view>

class Error
{
public:
  Error();
  Error(const Error& e);
  Error(Error&& e);
  ~Error();

  enum class Type
  {
    None = 0,
    Errno = 1,
    Socket = 2,
    User = 3,
    Win32 = 4,
    HResult = 5,
  };

  ALWAYS_INLINE Type GetType() const { return m_type; }
  ALWAYS_INLINE bool IsValid() const { return (m_type != Type::None); }
  ALWAYS_INLINE const std::string& GetDescription() const { return m_description; }

  void Clear();

  /// Error that is set by system functions, such as open().
  void SetErrno(int err);
  void SetErrno(std::string_view prefix, int err);

  /// Error that is set by socket functions, such as socket(). On Unix this is the same as errno.
  void SetSocket(int err);
  void SetSocket(std::string_view prefix, int err);

  /// Set both description and message.
  void SetString(std::string description);
  void SetStringView(std::string_view description);

#ifdef _WIN32
  /// Error that is returned by some Win32 functions, such as RegOpenKeyEx. Also used by other APIs through
  /// GetLastError().
  void SetWin32(unsigned long err);
  void SetWin32(std::string_view prefix, unsigned long err);

  /// Error that is returned by Win32 COM methods, e.g. S_OK.
  void SetHResult(long err);
  void SetHResult(std::string_view prefix, long err);
#endif

  static Error CreateNone();
  static Error CreateErrno(int err);
  static Error CreateSocket(int err);
  static Error CreateString(std::string description);
#ifdef _WIN32
  static Error CreateWin32(unsigned long err);
  static Error CreateHResult(long err);
#endif

  // helpers for setting
  static void SetErrno(Error* errptr, int err);
  static void SetErrno(Error* errptr, std::string_view prefix, int err);
  static void SetSocket(Error* errptr, int err);
  static void SetSocket(Error* errptr, std::string_view prefix, int err);
  static void SetString(Error* errptr, std::string description);
  static void SetStringView(Error* errptr, std::string_view description);

#ifdef _WIN32
  static void SetWin32(Error* errptr, unsigned long err);
  static void SetWin32(Error* errptr, std::string_view prefix, unsigned long err);
  static void SetHResult(Error* errptr, long err);
  static void SetHResult(Error* errptr, std::string_view prefix, long err);
#endif

  /// Sets a formatted message.
  template<typename... T>
  static void SetStringFmt(Error* errptr, fmt::format_string<T...> fmt, T&&... args)
  {
    if (errptr)
      Error::SetString(errptr, fmt::vformat(fmt, fmt::make_format_args(args...)));
  }

  void AddPrefix(std::string_view prefix);
  void AddSuffix(std::string_view suffix);
  static void AddPrefix(Error* errptr, std::string_view prefix);
  static void AddSuffix(Error* errptr, std::string_view prefix);

  template<typename... T>
  void AddPrefixFmt(fmt::format_string<T...> fmt, T&&... args)
  {
    AddPrefix(TinyString::from_vformat(fmt::string_view(fmt), fmt::make_format_args(args...)));
  }

  template<typename... T>
  void AddSuffixFmt(fmt::format_string<T...> fmt, T&&... args)
  {
    AddSuffix(TinyString::from_vformat(fmt::string_view(fmt), fmt::make_format_args(args...)));
  }

  template<typename... T>
  static void AddPrefixFmt(Error* errptr, fmt::format_string<T...> fmt, T&&... args)
  {
    if (errptr)
      Error::AddPrefix(errptr, TinyString::from_vformat(fmt::string_view(fmt), fmt::make_format_args(args...)));
  }
  template<typename... T>
  static void AddSuffixFmt(Error* errptr, fmt::format_string<T...> fmt, T&&... args)
  {
    if (errptr)
      Error::AddSuffix(errptr, TinyString::from_vformat(fmt::string_view(fmt), fmt::make_format_args(args...)));
  }

  Error& operator=(const Error& e);
  Error& operator=(Error&& e);
  bool operator==(const Error& e) const;
  bool operator!=(const Error& e) const;

private:
  Type m_type = Type::None;
  std::string m_description;
};
