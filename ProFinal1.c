#include"hexEditor.c"

void listaparticion(int hex, char sistema[]);
unsigned long valorhex(int , int, char *);
void CHSvalores(int [], int, char * );
void MostrarDatos(char *map, int pag);
void MostrarParticion(char *);


int main(int argc, char *argv[])
{
  if(argc!=2)
    printf("ERROR");
  else{

   //Funciones pa que funcione
   initscr();
   raw();
   noecho();
   cbreak();
   keypad(stdscr, TRUE);

   //Abrir Mapa
   char *map = mapFile(argv[1]);
     if (map == NULL) {
     endwin();
      exit(EXIT_FAILURE);
      }

   //Guardar el original para guardarlo
   char *map_origin=map;
   map+=446;

   char linea[64];
   int c, n=1;
   int en=0;
   unsigned long iniciosector;

   system("clear");
   sprintf(linea,"Particiones del disco: %s", argv[1]);
   mvprintw(0,12,linea);

   MostrarDatos(map,n);
   do{
    c = leeChar();
      switch(c) {
         case KEY_RIGHT:
            n = (n<4) ? n+1 : 1;
            break;
         case KEY_LEFT:
            n = (n>1) ? n - 1:4;
             break;
         case 10:
           iniciosector=valorhex(8, 11, map+(16*(n-1)));
           MostrarParticion(map_origin +(iniciosector*512));
            en++;
            break;
         default:
            // Nothing
            break;
      }

      if(en==0)
       MostrarDatos(map+(16*(n-1)),n);

 }while(c!=10);

    //pantalla(map,0);

     //Para terminar las funciones que inician y no se que hacen :v
    cerrarmap(map_origin);
    endwin();
    }
    return 0;

}

void listaparticion(int hex, char sistema[])
{
int primero = 131;
int segundo = 138;
int tercero = 7;
int cuarto = 168;
int quinto = 171;

if(hex==primero){
char tipo[32] = "linux native";
strcpy(sistema, tipo);
//printf("Cadena copiada: %s \n", sistema);
}


if(hex==segundo){
char tipo[32] = "linux Kernel";
strcpy(sistema, tipo);
//printf("Cadena copiada: %s \n", sistema);
}


if(hex==tercero){
char tipo[32] = "Windows";
strcpy(sistema, tipo);
//printf("Cadena copiada: %s \n", sistema);
}


if(hex== cuarto){
char tipo[32] = "Mac Os-x";
strcpy(sistema, tipo);
//printf("Cadena copiada: %s \n", sistema);
}


if(hex== quinto){
char tipo[32] = "Mac Os-x boot";
strcpy(sistema, tipo);
//printf("Cadena copiada: %s \n", sistema);
}

}

unsigned long valorhex(int ini, int fin, char *map){

 unsigned long Cant=0;

 int au;
 int j=1;
 for(int i=ini; i<=fin; i++){
     au=(int)map[i];
     if(au<0)
       au+=256;
     Cant+=(au *(j));
     j*=256;
   }

 return Cant;
}

void CHSvalores(int CHS[], int inicio, char *map){
  int aux;
  int a0, a1;

     //Head
     aux=(int)map[inicio];
     if(aux<0)
       aux+=256;
     CHS[1]=aux;

     //Sector
     aux=(int)map[inicio +1];
     if(aux<0)
       aux+=256;
     a0=aux/128;
     a1=aux/64;

     if(a1>=2)
       a1-=2;
     aux=aux-((a0*128) +(a1*64));
     CHS[2]=aux;

     //Cylinder
     aux=(int)map[inicio +2];
     if(aux<0)
       aux+=256;
     aux=aux+((a0*512) +(a1*256));
     CHS[0]=aux;
}


void MostrarDatos(char *map, int pag){

   //Variables de particion
   int aux;
   int CHSinicio[3];
   char tipoparticion[32];
   int CHSfin[3];
   unsigned long iniciosector;
   unsigned long cantidadsectores;
   char l[128];

   sprintf(l, "Particion %d", pag);
       mvprintw(1,5,l);

       //Rosa
       aux=(int)map[0];
       if(aux!=0)
        sprintf(l,"Arranca la particion");
       else
        sprintf(l,"No arranca la particion");

       mvprintw(2,0,l);
       //Amarillo
       CHSvalores(CHSinicio, 1, map);
       sprintf(l,"Inicio de la particion:\n\tHead:%10d\n\tSector: %10d\n\tCilindro: %10d", CHSinicio[1], CHSinicio[2], CHSinicio[0]);
       mvprintw(3,0,l);

       //Azul
       aux=(int)map[4];
       if(aux<0)
         aux+=256;
       listaparticion(aux,tipoparticion);
       sprintf(l,"Tipo particion: %s", tipoparticion);
       mvprintw(7,0,l);

       //Verde
        CHSvalores(CHSfin, 5, map);
       sprintf(l,"Fin de la particion:\n\tHead:%10d\n\tSector: %10d\n\tCilindro: %10d\n", CHSfin[1], CHSfin[2], CHSfin[0]);
       mvprintw(8,0,l);

       //Naranja

       iniciosector=valorhex(8, 11, map);
       sprintf(l,"Inicio de sectores: %10ld", iniciosector);
       mvprintw(12,0,l);

       //Verde oscuro

       cantidadsectores=valorhex(12, 15, map);
       sprintf(l, "Cantidad de sectores: %10ld\n", cantidadsectores);
       mvprintw(13,0,l);

   refresh();
}

void MostrarParticion(char *map){
  system("clear");

  //13 Byte Número de sectores por cluster

  int numsec=map[13];
  if(numsec<0)
    numsec+=256;

  //28 Dword (4 Bytes) Sectores ocultos

  double Sec_ocultos=*(double *)&map[28];

  //48  Long long (8Bytes) Dirección MFT

  long long DMFT= *(long long *)&map[48];

  //56  Long Long (8Bytes) Dirección MFT copia

  long long DMFTcopia= *(long long *)&map[56];

  //40 Long Long (8Bytes) Número de sectores del disco

  long long numsecdisco= *(long long *)&map[40];

  //72 Long Long (8Bytes) Etiqueta del Volumen

  long long Etiqueta= *(long long *)&map[72];

  //3 Long Long (8Bytes) ID Sistema

  char IDsistema[8]="NTFS";

  mvprintw(0,0,"Informacion del Disco");
  mvprintw(1,0,"Numero Sectores por cluster: %d", numsec);
  mvprintw(2,0,"Sectores ocultos: %d",Sec_ocultos);
  mvprintw(3,0,"Direccion MFT: %d",DMFT);
  mvprintw(4,0,"Direccion MTF copia: %d",DMFTcopia);
  mvprintw(5,0,"Numeros de sectores del disco: %d",numsecdisco);
  mvprintw(6,0,"Etiqueta del Volumen: %d",Etiqueta);
  mvprintw(7,0,"ID del Sistema: %s",IDsistema);

  refresh();
}
