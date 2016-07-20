/* strftime implementation for WEC7 
 *
 * Taken from newlib ()
 *
 * Original Author:     G. Haley
 * Additions from:      Eric Blake
 * Changes to allow dual use as wcstime, also:  Craig Howland
 *
 */

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS /* Disable deprecation warning in VS2008 */
#endif
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define YEAR_BASE 1900

#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

#define _CONST const
static _CONST int dname_len[7] =
{6, 6, 7, 9, 8, 6, 8};

static _CONST char *_CONST dname[7] =
{"Sunday", "Monday", "Tuesday", "Wednesday",
 "Thursday", "Friday", "Saturday"};

static _CONST int mname_len[12] =
{7, 8, 5, 5, 3, 4, 4, 6, 9, 7, 8, 8};

static _CONST char *_CONST mname[12] =
{"January", "February", "March", "April",
 "May", "June", "July", "August", "September", "October", "November",
 "December"};

/* Using the tm_year, tm_wday, and tm_yday components of TIM_P, return
   -1, 0, or 1 as the adjustment to add to the year for the ISO week
   numbering used in "%g%G%V", avoiding overflow.  */
static int iso_year_adjust(_CONST struct tm *tim_p)
{
  /* Account for fact that tm_year==0 is year 1900.  */
  int leap = isleap(tim_p->tm_year + (YEAR_BASE
				       - (tim_p->tm_year < 0 ? 0 : 2000)));

  /* Pack the yday, wday, and leap year into a single int since there are so
     many disparate cases.  */
#define PACK(yd, wd, lp) (((yd) << 4) + (wd << 1) + (lp))
  switch (PACK (tim_p->tm_yday, tim_p->tm_wday, leap))
    {
    case PACK (0, 5, 0): /* Jan 1 is Fri, not leap.  */
    case PACK (0, 6, 0): /* Jan 1 is Sat, not leap.  */
    case PACK (0, 0, 0): /* Jan 1 is Sun, not leap.  */
    case PACK (0, 5, 1): /* Jan 1 is Fri, leap year.  */
    case PACK (0, 6, 1): /* Jan 1 is Sat, leap year.  */
    case PACK (0, 0, 1): /* Jan 1 is Sun, leap year.  */
    case PACK (1, 6, 0): /* Jan 2 is Sat, not leap.  */
    case PACK (1, 0, 0): /* Jan 2 is Sun, not leap.  */
    case PACK (1, 6, 1): /* Jan 2 is Sat, leap year.  */
    case PACK (1, 0, 1): /* Jan 2 is Sun, leap year.  */
    case PACK (2, 0, 0): /* Jan 3 is Sun, not leap.  */
    case PACK (2, 0, 1): /* Jan 3 is Sun, leap year.  */
      return -1; /* Belongs to last week of previous year.  */
    case PACK (362, 1, 0): /* Dec 29 is Mon, not leap.  */
    case PACK (363, 1, 1): /* Dec 29 is Mon, leap year.  */
    case PACK (363, 1, 0): /* Dec 30 is Mon, not leap.  */
    case PACK (363, 2, 0): /* Dec 30 is Tue, not leap.  */
    case PACK (364, 1, 1): /* Dec 30 is Mon, leap year.  */
    case PACK (364, 2, 1): /* Dec 30 is Tue, leap year.  */
    case PACK (364, 1, 0): /* Dec 31 is Mon, not leap.  */
    case PACK (364, 2, 0): /* Dec 31 is Tue, not leap.  */
    case PACK (364, 3, 0): /* Dec 31 is Wed, not leap.  */
    case PACK (365, 1, 1): /* Dec 31 is Mon, leap year.  */
    case PACK (365, 2, 1): /* Dec 31 is Tue, leap year.  */
    case PACK (365, 3, 1): /* Dec 31 is Wed, leap year.  */
      return 1; /* Belongs to first week of next year.  */
    }
  return 0; /* Belongs to specified year.  */
#undef PACK
}

/*******************************************************************************
* wceex_strftime - Format date and time
*
* Description:
*   This function is similar to the sprintf function, but the conversion specifications
*   that can appear in the format template template are specialized for printing components
*   of the date and time brokentime according to the locale currently specified for time
*   conversion (see Locales).
*
* Reference:
*   The GNU C Library
*
*******************************************************************************/
size_t wceex_strftime(char *s, size_t maxsize, const char *format, const struct tm *tim_p)
{
  size_t count = 0;
  int i;

  for (;;)
    {
      while (*format && *format != '%')
	{
	  if (count < maxsize - 1)
	    s[count++] = *format++;
	  else
	    return 0;
	}

      if (*format == '\0')
	break;

      format++;
      if (*format == 'E' || *format == 'O')
	format++;

      switch (*format)
	{
	case 'a':
	  for (i = 0; i < 3; i++)
	    {
	      if (count < maxsize - 1)
		s[count++] =
		  dname[tim_p->tm_wday][i];
	      else
		return 0;
	    }
	  break;
	case 'A':
	  for (i = 0; i < dname_len[tim_p->tm_wday]; i++)
	    {
	      if (count < maxsize - 1)
		s[count++] =
		  dname[tim_p->tm_wday][i];
	      else
		return 0;
	    }
	  break;
	case 'b':
	case 'h':
	  for (i = 0; i < 3; i++)
	    {
	      if (count < maxsize - 1)
		s[count++] =
		  mname[tim_p->tm_mon][i];
	      else
		return 0;
	    }
	  break;
	case 'B':
	  for (i = 0; i < mname_len[tim_p->tm_mon]; i++)
	    {
	      if (count < maxsize - 1)
		s[count++] =
		  mname[tim_p->tm_mon][i];
	      else
		return 0;
	    }
	  break;
	case 'c':
	  {
	    /* Length is not known because of %C%y, so recurse. */
	    size_t adjust = wceex_strftime (&s[count], maxsize - count,
				      "%a %b %e %H:%M:%S %C%y", tim_p);
	    if (adjust > 0)
	      count += adjust;
	    else
	      return 0;
	  }
	  break;
	case 'C':
	  {
	    /* Examples of (tm_year + YEAR_BASE) that show how %Y == %C%y
	       with 32-bit int.
	       %Y		%C		%y
	       2147485547	21474855	47
	       10000		100		00
	       9999		99		99
	       0999		09		99
	       0099		00		99
	       0001		00		01
	       0000		00		00
	       -001		-0		01
	       -099		-0		99
	       -999		-9		99
	       -1000		-10		00
	       -10000		-100		00
	       -2147481748	-21474817	48

	       Be careful of both overflow and sign adjustment due to the
	       asymmetric range of years.
	    */
	    int neg = tim_p->tm_year < -YEAR_BASE;
	    int century = tim_p->tm_year >= 0
	      ? tim_p->tm_year / 100 + YEAR_BASE / 100
	      : abs (tim_p->tm_year + YEAR_BASE) / 100;
            count += _snprintf (&s[count], maxsize - count, "%s%.*d",
                               neg ? "-" : "", 2 - neg, century);
            if (count >= maxsize)
              return 0;
	  }
	  break;
	case 'd':
	case 'e':
	  if (count < maxsize - 2)
	    {
	      sprintf (&s[count], *format == 'd' ? "%.2d" : "%2d",
		       tim_p->tm_mday);
	      count += 2;
	    }
	  else
	    return 0;
	  break;
	case 'D':
	case 'x':
	  /* %m/%d/%y */
	  if (count < maxsize - 8)
	    {
	      sprintf (&s[count], "%.2d/%.2d/%.2d",
		       tim_p->tm_mon + 1, tim_p->tm_mday,
		       tim_p->tm_year >= 0 ? tim_p->tm_year % 100
		       : abs (tim_p->tm_year + YEAR_BASE) % 100);
	      count += 8;
	    }
	  else
	    return 0;
	  break;
        case 'F':
	  {
	    /* Length is not known because of %C%y, so recurse. */
	    size_t adjust = wceex_strftime (&s[count], maxsize - count,
				      "%C%y-%m-%d", tim_p);
	    if (adjust > 0)
	      count += adjust;
	    else
	      return 0;
	  }
          break;
        case 'g':
	  if (count < maxsize - 2)
	    {
	      /* Be careful of both overflow and negative years, thanks to
		 the asymmetric range of years.  */
	      int adjust = iso_year_adjust (tim_p);
	      int year = tim_p->tm_year >= 0 ? tim_p->tm_year % 100
		: abs (tim_p->tm_year + YEAR_BASE) % 100;
	      if (adjust < 0 && tim_p->tm_year <= -YEAR_BASE)
		adjust = 1;
	      else if (adjust > 0 && tim_p->tm_year < -YEAR_BASE)
		adjust = -1;
	      sprintf (&s[count], "%.2d",
		       ((year + adjust) % 100 + 100) % 100);
	      count += 2;
	    }
	  else
	    return 0;
          break;
        case 'G':
	  {
	    /* See the comments for 'C' and 'Y'; this is a variable length
	       field.  Although there is no requirement for a minimum number
	       of digits, we use 4 for consistency with 'Y'.  */
	    int neg = tim_p->tm_year < -YEAR_BASE;
	    int adjust = iso_year_adjust (tim_p);
	    int century = tim_p->tm_year >= 0
	      ? tim_p->tm_year / 100 + YEAR_BASE / 100
	      : abs (tim_p->tm_year + YEAR_BASE) / 100;
	    int year = tim_p->tm_year >= 0 ? tim_p->tm_year % 100
	      : abs (tim_p->tm_year + YEAR_BASE) % 100;
	    if (adjust < 0 && tim_p->tm_year <= -YEAR_BASE)
	      neg = adjust = 1;
	    else if (adjust > 0 && neg)
	      adjust = -1;
	    year += adjust;
	    if (year == -1)
	      {
		year = 99;
		--century;
	      }
	    else if (year == 100)
	      {
		year = 0;
		++century;
	      }
            count += _snprintf (&s[count], maxsize - count, "%s%.*d%.2d",
                               neg ? "-" : "", 2 - neg, century, year);
            if (count >= maxsize)
              return 0;
	  }
          break;
	case 'H':
	case 'k':
	  if (count < maxsize - 2)
	    {
	      sprintf (&s[count], *format == 'k' ? "%2d" : "%.2d",
		       tim_p->tm_hour);
	      count += 2;
	    }
	  else
	    return 0;
	  break;
	case 'I':
	case 'l':
	  if (count < maxsize - 2)
	    {
	      if (tim_p->tm_hour == 0 ||
		  tim_p->tm_hour == 12)
		{
		  s[count++] = '1';
		  s[count++] = '2';
		}
	      else
		{
		  sprintf (&s[count], *format == 'I' ? "%.2d" : "%2d",
			   tim_p->tm_hour % 12);
		  count += 2;
		}
	    }
	  else
	    return 0;
	  break;
	case 'j':
	  if (count < maxsize - 3)
	    {
	      sprintf (&s[count], "%.3d",
		       tim_p->tm_yday + 1);
	      count += 3;
	    }
	  else
	    return 0;
	  break;
	case 'm':
	  if (count < maxsize - 2)
	    {
	      sprintf (&s[count], "%.2d",
		       tim_p->tm_mon + 1);
	      count += 2;
	    }
	  else
	    return 0;
	  break;
	case 'M':
	  if (count < maxsize - 2)
	    {
	      sprintf (&s[count], "%.2d",
		       tim_p->tm_min);
	      count += 2;
	    }
	  else
	    return 0;
	  break;
	case 'n':
	  if (count < maxsize - 1)
	    s[count++] = '\n';
	  else
	    return 0;
	  break;
	case 'p':
	  if (count < maxsize - 2)
	    {
	      if (tim_p->tm_hour < 12)
		s[count++] = 'A';
	      else
		s[count++] = 'P';

	      s[count++] = 'M';
	    }
	  else
	    return 0;
	  break;
	case 'r':
	  if (count < maxsize - 11)
	    {
	      if (tim_p->tm_hour == 0 ||
		  tim_p->tm_hour == 12)
		{
		  s[count++] = '1';
		  s[count++] = '2';
		}
	      else
		{
		  sprintf (&s[count], "%.2d", tim_p->tm_hour % 12);
		  count += 2;
		}
	      s[count++] = ':';
	      sprintf (&s[count], "%.2d",
		       tim_p->tm_min);
	      count += 2;
	      s[count++] = ':';
	      sprintf (&s[count], "%.2d",
		       tim_p->tm_sec);
	      count += 2;
	      s[count++] = ' ';
	      if (tim_p->tm_hour < 12)
		s[count++] = 'A';
	      else
		s[count++] = 'P';

	      s[count++] = 'M';
	    }
	  else
	    return 0;
	  break;
        case 'R':
          if (count < maxsize - 5)
            {
              sprintf (&s[count], "%.2d:%.2d", tim_p->tm_hour, tim_p->tm_min);
              count += 5;
            }
          else
            return 0;
          break;
	case 'S':
	  if (count < maxsize - 2)
	    {
	      sprintf (&s[count], "%.2d",
		       tim_p->tm_sec);
	      count += 2;
	    }
	  else
	    return 0;
	  break;
	case 't':
	  if (count < maxsize - 1)
	    s[count++] = '\t';
	  else
	    return 0;
	  break;
        case 'T':
        case 'X':
          if (count < maxsize - 8)
            {
              sprintf (&s[count], "%.2d:%.2d:%.2d", tim_p->tm_hour,
                       tim_p->tm_min, tim_p->tm_sec);
              count += 8;
            }
          else
            return 0;
          break;
        case 'u':
          if (count < maxsize - 1)
            {
              if (tim_p->tm_wday == 0)
                s[count++] = '7';
              else
                s[count++] = '0' + tim_p->tm_wday;
            }
          else
            return 0;
          break;
	case 'U':
	  if (count < maxsize - 2)
	    {
	      sprintf (&s[count], "%.2d",
		       (tim_p->tm_yday + 7 -
			tim_p->tm_wday) / 7);
	      count += 2;
	    }
	  else
	    return 0;
	  break;
        case 'V':
	  if (count < maxsize - 2)
	    {
	      int adjust = iso_year_adjust (tim_p);
	      int wday = (tim_p->tm_wday) ? tim_p->tm_wday - 1 : 6;
	      int week = (tim_p->tm_yday + 10 - wday) / 7;
	      if (adjust > 0)
		week = 1;
	      else if (adjust < 0)
		/* Previous year has 53 weeks if current year starts on
		   Fri, and also if current year starts on Sat and
		   previous year was leap year.  */
		week = 52 + (4 >= (wday - tim_p->tm_yday
				   - isleap (tim_p->tm_year
					     + (YEAR_BASE - 1
						- (tim_p->tm_year < 0
						   ? 0 : 2000)))));
	      sprintf (&s[count], "%.2d", week);
	      count += 2;
	    }
	  else
	    return 0;
          break;
	case 'w':
	  if (count < maxsize - 1)
            s[count++] = '0' + tim_p->tm_wday;
	  else
	    return 0;
	  break;
	case 'W':
	  if (count < maxsize - 2)
	    {
	      int wday = (tim_p->tm_wday) ? tim_p->tm_wday - 1 : 6;
	      sprintf (&s[count], "%.2d",
		       (tim_p->tm_yday + 7 - wday) / 7);
	      count += 2;
	    }
	  else
	    return 0;
	  break;
	case 'y':
	  if (count < maxsize - 2)
	    {
	      /* Be careful of both overflow and negative years, thanks to
		 the asymmetric range of years.  */
	      int year = tim_p->tm_year >= 0 ? tim_p->tm_year % 100
		: abs (tim_p->tm_year + YEAR_BASE) % 100;
	      sprintf (&s[count], "%.2d", year);
	      count += 2;
	    }
	  else
	    return 0;
	  break;
	case 'Y':
	  {
	    /* Length is not known because of %C%y, so recurse. */
	    size_t adjust = wceex_strftime (&s[count], maxsize - count,
				      "%C%y", tim_p);
	    if (adjust > 0)
	      count += adjust;
	    else
	      return 0;
	  }
	  break;
        case 'z':
	  break;
	case 'Z':
	  break;
	case '%':
	  if (count < maxsize - 1)
	    s[count++] = '%';
	  else
	    return 0;
	  break;
	}
      if (*format)
	format++;
      else
	break;
    }
  if (maxsize)
    s[count] = '\0';

  return count;
}
