#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <curses.h>
#include <sys/stat.h>
#include <sys/mman.h>

/* Variable global para mejor legibilidad */
int fd; // Archivo a leer

char *hazLinea(char *base, int dir) {
char linea[100]; // La linea es mas pequeÃ±a
int o=0;
// Muestra 16 caracteres por cada linea
o += sprintf(linea,"%08x ",dir); // offset en hexadecimal
for(int i=0; i < 4; i++) {
unsigned char a,b,c,d;
a = base[dir+4*i+0];
b = base[dir+4*i+1];
c = base[dir+4*i+2];
d = base[dir+4*i+3];
o += sprintf(&linea[o],"%02x %02x %02x %02x ", a, b, c, d);
}
for(int i=0; i < 16; i++) {
if (isprint(base[dir+i])) {
o += sprintf(&linea[o],"%c",base[dir+i]);
}
else {
o += sprintf(&linea[o],".");
}
}
sprintf(&linea[o],"\n");
return(strdup(linea));
}

char *mapFile(char *filePath) {
    /* Abre archivo */
    fd = open(filePath, O_RDWR);
    if (fd == -1) {
    perror("Error abriendo el archivo");
   return(NULL);
    }

    /* Mapea archivo */
    struct stat st;
    fstat(fd,&st);
    int fs = st.st_size;

    char *map = mmap(0, fs, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
    close(fd);
   perror("Error mapeando el archivo");
   return(NULL);
    }

  return map;
}

int leeChar() {
  int chars[5];
  int ch,i=0;
  nodelay(stdscr, TRUE);
  while((ch = getch()) == ERR); /* Espera activa */
  ungetch(ch);
  while((ch = getch()) != ERR) {
    chars[i++]=ch;
  }
  /* convierte a numero con todo lo leido */
  int res=0;
  for(int j=0;j<i;j++) {
    res <<=8;
    res |= chars[j];
  }
  return res;
}


void pantalla(char *map, int offset){
      for(int i= 0; i<25; i++) {
    // Haz linea, base y offset
    char *l = hazLinea(map,offset*16 + i*16);
   mvprintw(i,0,l);
    }
    refresh();
}

int edita(char *filename) {

    /* Lee archivo */
    char *map = mapFile(filename);
    if (map == NULL) {
      exit(EXIT_FAILURE);
      }

    int offset=0;

    pantalla(map, offset);

int x=1, y=0, c=0;
int valor=0;
char buf[4];
   do {
    move(y,8+x);

    c = leeChar();
      switch(c) {
         case KEY_UP:
         if(y>0)
         y-=1;
         else if(offset!=0){
         offset--;
         pantalla(map, offset);
         }
         //y = (y>0) ? y - 1 : 24;
            break;
         case KEY_DOWN:
            if(y<24)
            y+=1;
            else{
            offset++;
            pantalla(map, offset);
            }
            //y = (y<24) ? y + 1 : 0;
            break;
         case KEY_RIGHT:
            x = (x<64) ? ((x>=49) ? x+1: x+3) : 1;
            break;
         case KEY_LEFT:
         x = (x>1) ? ((x>49) ? x-1: x-3) : 64;
            break;
         case 24:
            //Nothing
           break;
         default:
             if(x>=49)
              map[(y*16)+(x-49)+(offset*16)]=c;
             else{
             buf[0]=c;
             c=leeChar();
             buf[1]=c;
             buf[2]='\n';
             valor=strtol(buf, NULL, 16);
             if(valor!=0)
              {
              map[(y*16)+(x/3)+(offset*16)]=valor;
              }
             }
             pantalla(map, offset);
            break;
      }


   } while (c != 24);

    if (munmap(map, fd) == -1) {
      perror("Error al desmapear");
    }
    close(fd);

   return 0;

}

void cerrarmap(char *map){
    if (munmap(map, fd) == -1) {
      perror("Error al desmapear");
    }
    close(fd);
}
