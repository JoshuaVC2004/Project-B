/* kernel.c - Joshua - Project B */

void printString(char*);
void printChar(char);
void readString(char*);
void readSector(char*, int);
void handleInterrupt21(int, int, int, int);
void makeInterrupt21();
int interrupt(int, int, int, int, int);

int main()
{
    char line[80];
    char buffer[512];

    makeInterrupt21();

    interrupt(0x21, 0, "Enter a line: ", 0, 0);
    interrupt(0x21, 1, line, 0, 0);
    interrupt(0x21, 0, line, 0, 0);

    interrupt(0x21, 2, buffer, 30, 0);
    interrupt(0x21, 0, buffer, 0, 0);

    while (1)
    {
    }
}

void printChar(char c)
{
    interrupt(0x10, 0xe * 256 + c, 0, 0, 0);
}

void printString(char* chars)
{
    int i = 0;

    while (chars[i] != 0x0)
    {
        printChar(chars[i]);
        i++;
    }
}

void readString(char* buf)
{
    int i = 0;
    char c;

    while (1)
    {
        c = interrupt(0x16, 0, 0, 0, 0);

        if (c == 0xd)
        {
            break;
        }

        if (c == 0x8)
        {
            if (i > 0)
            {
                i--;
                printChar(0x8);
                printChar(' ');
                printChar(0x8);
            }
        }
        else if (i < 78)
        {
            buf[i] = c;
            i++;
            printChar(c);
        }
    }

    buf[i] = 0xa;
    buf[i + 1] = 0x0;
    printChar(0xd);
    printChar(0xa);
}

void readSector(char* buffer, int sector)
{
    int ax = 2 * 256 + 1;               /* AH = 2 (read), AL = 1 sector */
    int cx = 0 * 256 + (sector + 1);    /* CH = track 0, CL = sector + 1 */
    int dx = 0 * 256 + 0x80;            /* DH = head 0, DL = hard disk */

    interrupt(0x13, ax, buffer, cx, dx);
}

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
    if (ax == 0)
    {
        printString((char*)bx);
    }
    else if (ax == 1)
    {
        readString((char*)bx);
    }
    else if (ax == 2)
    {
        readSector((char*)bx, cx);
    }
    else
    {
        printString("Error: invalid interrupt 21 function\r\n");
    }
}
