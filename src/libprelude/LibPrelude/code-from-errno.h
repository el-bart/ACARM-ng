/* Output of mkerrcodes2.awk.  DO NOT EDIT.  */

/* errnos.h - List of system error values.
   Copyright (C) 2004 g10 Code GmbH
   This file is part of libgpg-error.

   libgpg-error is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   libgpg-error is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with libgpg-error; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

static const int err_code_from_index[] = {
  PRELUDE_ERROR_EPERM,
  PRELUDE_ERROR_ENOENT,
  PRELUDE_ERROR_ESRCH,
  PRELUDE_ERROR_EINTR,
  PRELUDE_ERROR_EIO,
  PRELUDE_ERROR_ENXIO,
  PRELUDE_ERROR_E2BIG,
  PRELUDE_ERROR_ENOEXEC,
  PRELUDE_ERROR_EBADF,
  PRELUDE_ERROR_ECHILD,
  PRELUDE_ERROR_EAGAIN,
  PRELUDE_ERROR_EWOULDBLOCK,
  PRELUDE_ERROR_ENOMEM,
  PRELUDE_ERROR_EACCES,
  PRELUDE_ERROR_EFAULT,
  PRELUDE_ERROR_ENOTBLK,
  PRELUDE_ERROR_EBUSY,
  PRELUDE_ERROR_EEXIST,
  PRELUDE_ERROR_EXDEV,
  PRELUDE_ERROR_ENODEV,
  PRELUDE_ERROR_ENOTDIR,
  PRELUDE_ERROR_EISDIR,
  PRELUDE_ERROR_EINVAL,
  PRELUDE_ERROR_ENFILE,
  PRELUDE_ERROR_EMFILE,
  PRELUDE_ERROR_ENOTTY,
  PRELUDE_ERROR_ETXTBSY,
  PRELUDE_ERROR_EFBIG,
  PRELUDE_ERROR_ENOSPC,
  PRELUDE_ERROR_ESPIPE,
  PRELUDE_ERROR_EROFS,
  PRELUDE_ERROR_EMLINK,
  PRELUDE_ERROR_EPIPE,
  PRELUDE_ERROR_EDOM,
  PRELUDE_ERROR_ERANGE,
  PRELUDE_ERROR_EDEADLK,
  PRELUDE_ERROR_EDEADLOCK,
  PRELUDE_ERROR_ENAMETOOLONG,
  PRELUDE_ERROR_ENOLCK,
  PRELUDE_ERROR_ENOSYS,
  PRELUDE_ERROR_ENOTEMPTY,
  PRELUDE_ERROR_ELOOP,
  PRELUDE_ERROR_ENOMSG,
  PRELUDE_ERROR_EIDRM,
  PRELUDE_ERROR_ECHRNG,
  PRELUDE_ERROR_EL2NSYNC,
  PRELUDE_ERROR_EL3HLT,
  PRELUDE_ERROR_EL3RST,
  PRELUDE_ERROR_ELNRNG,
  PRELUDE_ERROR_EUNATCH,
  PRELUDE_ERROR_ENOCSI,
  PRELUDE_ERROR_EL2HLT,
  PRELUDE_ERROR_EBADE,
  PRELUDE_ERROR_EBADR,
  PRELUDE_ERROR_EXFULL,
  PRELUDE_ERROR_ENOANO,
  PRELUDE_ERROR_EBADRQC,
  PRELUDE_ERROR_EBADSLT,
  PRELUDE_ERROR_EBFONT,
  PRELUDE_ERROR_ENOSTR,
  PRELUDE_ERROR_ENODATA,
  PRELUDE_ERROR_ETIME,
  PRELUDE_ERROR_ENOSR,
  PRELUDE_ERROR_ENONET,
  PRELUDE_ERROR_ENOPKG,
  PRELUDE_ERROR_EREMOTE,
  PRELUDE_ERROR_ENOLINK,
  PRELUDE_ERROR_EADV,
  PRELUDE_ERROR_ESRMNT,
  PRELUDE_ERROR_ECOMM,
  PRELUDE_ERROR_EPROTO,
  PRELUDE_ERROR_EMULTIHOP,
  PRELUDE_ERROR_EDOTDOT,
  PRELUDE_ERROR_EBADMSG,
  PRELUDE_ERROR_EOVERFLOW,
  PRELUDE_ERROR_ENOTUNIQ,
  PRELUDE_ERROR_EBADFD,
  PRELUDE_ERROR_EREMCHG,
  PRELUDE_ERROR_ELIBACC,
  PRELUDE_ERROR_ELIBBAD,
  PRELUDE_ERROR_ELIBSCN,
  PRELUDE_ERROR_ELIBMAX,
  PRELUDE_ERROR_ELIBEXEC,
  PRELUDE_ERROR_EILSEQ,
  PRELUDE_ERROR_ERESTART,
  PRELUDE_ERROR_ESTRPIPE,
  PRELUDE_ERROR_EUSERS,
  PRELUDE_ERROR_ENOTSOCK,
  PRELUDE_ERROR_EDESTADDRREQ,
  PRELUDE_ERROR_EMSGSIZE,
  PRELUDE_ERROR_EPROTOTYPE,
  PRELUDE_ERROR_ENOPROTOOPT,
  PRELUDE_ERROR_EPROTONOSUPPORT,
  PRELUDE_ERROR_ESOCKTNOSUPPORT,
  PRELUDE_ERROR_ENOTSUP,
  PRELUDE_ERROR_EOPNOTSUPP,
  PRELUDE_ERROR_EPFNOSUPPORT,
  PRELUDE_ERROR_EAFNOSUPPORT,
  PRELUDE_ERROR_EADDRINUSE,
  PRELUDE_ERROR_EADDRNOTAVAIL,
  PRELUDE_ERROR_ENETDOWN,
  PRELUDE_ERROR_ENETUNREACH,
  PRELUDE_ERROR_ENETRESET,
  PRELUDE_ERROR_ECONNABORTED,
  PRELUDE_ERROR_ECONNRESET,
  PRELUDE_ERROR_ENOBUFS,
  PRELUDE_ERROR_EISCONN,
  PRELUDE_ERROR_ENOTCONN,
  PRELUDE_ERROR_ESHUTDOWN,
  PRELUDE_ERROR_ETOOMANYREFS,
  PRELUDE_ERROR_ETIMEDOUT,
  PRELUDE_ERROR_ECONNREFUSED,
  PRELUDE_ERROR_EHOSTDOWN,
  PRELUDE_ERROR_EHOSTUNREACH,
  PRELUDE_ERROR_EALREADY,
  PRELUDE_ERROR_EINPROGRESS,
  PRELUDE_ERROR_ESTALE,
  PRELUDE_ERROR_EUCLEAN,
  PRELUDE_ERROR_ENOTNAM,
  PRELUDE_ERROR_ENAVAIL,
  PRELUDE_ERROR_EISNAM,
  PRELUDE_ERROR_EREMOTEIO,
  PRELUDE_ERROR_EDQUOT,
  PRELUDE_ERROR_ENOMEDIUM,
  PRELUDE_ERROR_EMEDIUMTYPE,
  PRELUDE_ERROR_ECANCELED,
};

#define errno_to_idx(code) (0 ? -1 \
  : ((code >= 1) && (code <= 11)) ? (code - 1) \
  : ((code >= 11) && (code <= 35)) ? (code - 0) \
  : ((code >= 35) && (code <= 40)) ? (code - -1) \
  : ((code >= 42) && (code <= 57)) ? (code - 0) \
  : ((code >= 59) && (code <= 95)) ? (code - 1) \
  : ((code >= 95) && (code <= 125)) ? (code - 0) \
  : -1)
/* public header */
