# Useful gdb string convenience functions.
# Copyright (C) 2012-2013 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""$_memeq, $_strlen, $_streq, $_regex"""

import gdb
import re


class _MemEq(gdb.Function):
  """$_memeq - compare bytes of memory

Usage:
  $_memeq(a, b, len)

Returns:
  True if len bytes at a and b compare equally.
"""
  def __init__(self):
    super(_MemEq, self).__init__("_memeq")

  def invoke(self, a, b, length):
    if length < 0:
      raise ValueError("length must be non-negative")
    if length == 0:
      return True
    # The argument(s) to vector are [low_bound,]high_bound.
    byte_vector = gdb.lookup_type("char").vector(length - 1)
    ptr_byte_vector = byte_vector.pointer()
    a_ptr = a.reinterpret_cast(ptr_byte_vector)
    b_ptr = b.reinterpret_cast(ptr_byte_vector)
    return a_ptr.dereference() == b_ptr.dereference()


class _StrLen(gdb.Function):
  """$_strlen - compute string length

Usage:
  $_strlen(a)

Returns:
  Length of string a, assumed to be a string in the current language.
"""
  def __init__(self):
    super(_StrLen, self).__init__("_strlen")

  def invoke(self, a):
    s = a.string()
    return len(s)


class _StrEq(gdb.Function):
  """$_streq - check string equality

Usage:
  $_streq(a, b)

Returns:
  True if a and b are identical strings in the current language.

Example (amd64-linux):
  catch syscall open
  cond $bpnum $_streq((char*) $rdi, "foo")
"""
  def __init__(self):
    super(_StrEq, self).__init__("_streq")

  def invoke(self, a, b):
    return a.string() == b.string()


class _RegEx(gdb.Function):
  """$_regex - check if a string matches a regular expression

Usage:
  $_regex(string, regex)

Returns:
  True if string str (in the current language) matches the
  regular expression regex.
"""
  def __init__(self):
    super(_RegEx, self).__init__("_regex")

  def invoke(self, string, regex):
    s = string.string()
    r = re.compile(regex.string())
    return bool(r.match(s))


# GDB will import us automagically via gdb/__init__.py.
_MemEq()
_StrLen()
_StrEq()
_RegEx()
