

#define	HOUR	 (60 * 60L)     /* # seconds in an hour */
#define	DAY	(24 * HOUR)     /* # seconds in a day */
#define	YEAR	(365 * DAY)     /* # seconds in a year */


static void set_time(const char *time_str);
static void to_time(long ct, int *year, int *month, int *day, 
                        int *hour, int *minute, int *second);
static long to_second(int year, int month, int day, int hour, 
                                        int minute, int second);
static int leap_year(int year);
static int str2num(const char *time_str, int *year, int *month, 
                        int *day, int *hour, int *min, int *sec);
static int a2i(const char *s);
static int d_isdigit(int c);
static long d_strlen(const char *s);


extern int stime(long *top);
extern long time(long *top);
extern int printf(const char *format, ...);


char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };


int main(int argc, char *argv[])
{
        int year, month, day;
        int hour, minute, second;
        long ct;

        if (argc == 2) {
                set_time(argv[1]);
        } else {
                time(&ct);
                to_time(ct, &year, &month, &day, &hour, &minute, &second);
                printf("%s %02d %d %02d:%02d:%02d\n", months[month-1], day, year, 
                                                        hour, minute, second);
        }
        return 0;
}

static void set_time(const char *time_str)
{
        int year, month, day;
        int hour, minute, second;
        long ct;

        str2num(time_str, &year, &month, &day, &hour, &minute, &second);
        ct = to_second(year, month, day, hour, minute, second);
        stime(&ct);
}

static void to_time(long ct, int *year, int *month, int *day, 
                        int *hour, int *minute, int *second)
{
        int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        long i, leap;

        // *year = *month = *day = *hour = *minute = *second = 0;
        
        for (i = 1970; ; i++) {
                if (leap_year(i)) {
                        leap = DAY; 
                } else {
                        leap = 0;
                }
                if (ct - YEAR - leap < 0) {
                       break; 
                }
                ct -= (YEAR + leap);
        }
        *year = i;
        
        if (leap_year(*year)) {
                days_per_month[1]++;
        }
        i = 0;
        while (ct >= (days_per_month[i] * DAY)) {
                ct -= (days_per_month[i++] * DAY);
        }
        *month = i+1;
        
        i = 1;
        while (ct >= DAY) {
                ct -= DAY;
                i++;
        }
        *day = i;
        
        i = 0;
        while (ct >= HOUR) {
                ct -= HOUR;
                i++;
        }
        *hour = i;
        
        i = 0;
        while (ct >= 60L) {
                ct -= 60L;
                i++;
        }
        *minute = i;
        
        
        *second = ct;
}

static long to_second(int year, int month, int day, int hour, 
                                        int minute, int second)
{
        int days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
        int i;
        long ct = 0;
        
        if (year < 70) {
                year += 2000;
        } else {
                year += 1900;
        }
        
        for (i = 1970; i < year; i++) {
                if (leap_year(i)) {
                        ct = ct + YEAR + DAY;
                } else {
                        ct += YEAR;
                }
        }
        
        ct = ct + days[month - 1] * DAY;
        if (leap_year(year) && month > 2) {
                ct = ct + DAY;
        }
        
        ct = ct + (day - 1) * DAY;
        ct += hour * HOUR;
        ct += minute * 60;
        ct += second;
        
        return ct;
}

static int leap_year(int year)
{
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
                return 1;
        else
                return 0;
}

static int str2num(const char *time_str, int *year, int *month, 
                        int *day, int *hour, int *min, int *sec)
{
        int i, j, c;
        
        *year = *month = *day = *hour = *min = *sec = 0;
        
        j = d_strlen(time_str);
        for (i = 0; i < j; i++) {
                c = *(time_str+i);
                if (!d_isdigit(c)) {
                        return 1;
                }
        }
        
        i = d_strlen(time_str);
        switch (i / 2) {
                case 6:
                        *sec = a2i(time_str + 10);

                case 5:
                        *min = a2i(time_str + 8);
                        *hour = a2i(time_str + 6);

                case 3:
                        *day = a2i(time_str + 2);
                        *month = a2i(time_str + 0);
                        *year = a2i(time_str + 4);
                        break;
                
                default:
                        return 1;
        }
        
        return 0;
}

static int a2i(const char *s)
{
        int i;
        
        i = (*s - '0') * 10;
        i = i + (*(s + 1) - '0');
        return i;
}

static int d_isdigit(int c)
{
        if (c >= '0' && c <= '9') {
                return 1;
        } else {
                return 0;
        }
}

static long d_strlen(const char *s)
{
        const char *sc;

        for (sc = s; *sc != '\0'; ++sc);

        return sc - s;
}
