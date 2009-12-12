/**
 * Running `g++ -lncurses -o cpc -DNCURSES cpc.cpp' works fine for me -- LK
 */
#include <vector>
#include <map>
#include <string.h>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#define CONSOLE_SUPPORT
   #ifdef CH_USE_UNICODE
     // Make sure we don't override addch for consolesupport.cpp,
     // because addch_unicode may use addch internally.
     #ifndef CONSOLE_SUPPORT
        #undef addch
        #define addch(a) addch_unicode(a)
     #endif
   #endif

using namespace std;

#define CH_USE_ASCII_HACK
#define CURSES_GRAPHICS

#include "../src/cursesgraphics.h"

 /* raw_output() is provided in PDcurses/Xcurses but is not in ncurses.
   * This function is for compatibility and is currently a do nothing function.
   */
 #ifdef NCURSES
 inline int raw_output(bool bf)
 {
    raw();
 return OK;
 }

 #endif

unsigned long picnum,dimx,dimy;
unsigned char bigletters[27][5][7][4];
unsigned char newstops[5][80][5][4];
unsigned char newspic[20][78][18][4];


int translateGraphicsChar(int c)
{
    // This will be no good for Unicode...
    int cin = c;
    int cout = cin;

//    if ((cin >= 0) && (cin <32))
//    {
//        cout = (cin | A_ALTCHARSET);
//    }
    if ((cin >= 0) && (cin < 256))
    {
        cout = gchar[cin].native_code;
        if (cout == 0)
        {
            cout = cin;
        }
    }
    return cout;
}

//sets current color to desired setting
void set_color(short f,short b,char bright,char blink=0)
{
   //color swap required for PDcurses
   if(f==7&&b==0)
   {
      f=0;b=0;
   }
   else if(f==0&&b==0)
   {
      f=7;b=0;
   }

   chtype blinky=0, brighty=0;
   
   if(blink)blinky=A_BLINK;
   if(bright)brighty=A_BOLD;

   //pick color pair based on foreground and background
   if(bright)attrset(brighty | blinky | COLOR_PAIR(f*8+b));
   else
   {
      attrset(COLOR_PAIR(f*8+b));
   }
}

void displaycenterednewsfont(char *str,int y)
{
   int width=-1;
   int s;
   for(s=0;s<strlen(str);s++)
   {
      if(str[s]>='A'&&str[s]<='Z')width+=6;
      else if(str[s]=='\'')width+=4;
      else width+=3;
   }

   int x=39-width/2;

   for(s=0;s<strlen(str);s++)
   {
      if((str[s]>='A'&&str[s]<='Z')||str[s]=='\'')
      {
         int p;
         if(str[s]>='A'&&str[s]<='Z')p=str[s]-'A';
         else p=26;
         int lim=6;
         if(str[s]=='\'')lim=4;
         if(s==strlen(str)-1)lim--;
         for(int x2=0;x2<lim;x2++)
         {
            for(int y2=0;y2<7;y2++)
            {
               move(y+y2,x+x2);

#ifdef NCURSES
               // Clean the square first.
               set_color(COLOR_BLACK, COLOR_BLACK, 0);
               addch(CH_FULL_BLOCK);
               move(y+y2,x+x2);
#endif

               if(x2==5)
               {
                  set_color(COLOR_WHITE,COLOR_BLACK,0);
                  addch(CH_FULL_BLOCK);
               }
               else
               {
                  set_color(bigletters[p][x2][y2][1],
                     bigletters[p][x2][y2][2],
                     bigletters[p][x2][y2][3]);
                  addch(translateGraphicsChar(bigletters[p][x2][y2][0]));
               }
            }
         }
         x+=lim;
      }
      else
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         for(int x2=0;x2<3;x2++)
         {
            for(int y2=0;y2<7;y2++)
            {
               move(y+y2,x+x2);
               addch(CH_FULL_BLOCK);
            }
         }
         x+=3;
      }
   }
}

void displaynewspicture(int p,int y)
{
   for(int x2=0;x2<78;x2++)
   {
      for(int y2=0;y2<15;y2++)
      {
         if(y+y2>24)break;
         move(y+y2,1+x2);
         set_color(newspic[p][x2][y2][1],
            newspic[p][x2][y2][2],
            newspic[p][x2][y2][3]);
         addch(translateGraphicsChar(newspic[p][x2][y2][0]));
      }
   }
}

void loadgraphics(void)
{
   int p, x, y;

   int numbytes;
   FILE *h;

   h = fopen("newspic.cpc", "rb");
   if(h!=NULL)
   {
      numbytes=fread(&picnum,sizeof(int),1,h);
      numbytes=fread(&dimx,sizeof(int),1,h);
      numbytes=fread(&dimy,sizeof(int),1,h);
      for(int p=0;p<picnum;p++)
      {
         for(int x=0;x<dimx;x++)
         {
            for(int y=0;y<dimy;y++)
            {
               numbytes=fread(&newspic[p][x][y][0],sizeof(char),4,h);
            }
         }
      }
   fclose(h);
   }

   h = fopen("newstops.cpc", "rb");
   if(h!=NULL)
   {
      numbytes=fread(&picnum,sizeof(int),1,h);
      numbytes=fread(&dimx,sizeof(int),1,h);
      numbytes=fread(&dimy,sizeof(int),1,h);
      for(int p=0;p<picnum;p++)
      {
         for(int x=0;x<dimx;x++)
         {
            for(int y=0;y<dimy;y++)
            {
               numbytes=fread(&newstops[p][x][y][0],sizeof(char),4,h);
            }
         }
      }
   fclose(h);
   }

   h = fopen("largecap.cpc", "rb");
   if(h!=NULL)
   {
      numbytes=fread(&picnum,sizeof(int),1,h);
      numbytes=fread(&dimx,sizeof(int),1,h);
      numbytes=fread(&dimy,sizeof(int),1,h);
      for(int p=0;p<picnum;p++)
      {
         for(int x=0;x<dimx;x++)
         {
            for(int y=0;y<dimy;y++)
            {
               numbytes=fread(&bigletters[p][x][y][0],sizeof(char),4,h);
            }
         }
      }
   fclose(h);
   }
}


int main(/*int nargs, char *args[]*/)
{
   initscr();
   noecho();
   //initialize curses color
   start_color();
   //initialize the array of color pairs
   for(int i=0;i<8;i++)
   {
      for(int j=0;j<8;j++)
      {
         if(i==0&&j==0)
         {
            init_pair(7*8,0,0);
            continue;
         }
         if(i==7&&j==0)continue;
         init_pair(i*8+j,i,j);
      }
   }
   //turns off cursor
   //curs_set(0);
   //begin the game loop
   keypad(stdscr,TRUE);
   raw_output(TRUE);
   loadgraphics();

   char c=0;
   char index=0;
   while(1)
   {
      // Render
      clear();
      move(0,0);
      set_color(COLOR_GREEN,COLOR_BLACK,1);
      addstr("Press ENTER to exit, or press [ or ] to browse pictures.");
      move(1,0);
      addstr("Currently displaying Liberal picture ");
      addch('0'+index);
      displaynewspicture(index,2);
      refresh();
      
      // Read input
      while(c!=10 && c!='[' && c!=']')
         c=getch();
      // Act on input
      if(c==10)/*exit(0);*/return 0;
      else if(c==']')index++;
      else if(c=='[' && index!=0)index--;
      if(index==-1)
         index=20;
      if(index==21)
         index=0;
      c=0;
   }
}