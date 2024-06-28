#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
using namespace std;

class Relacion
{
public:
    string nombre;
    int cantAtributos;
    vector<string> nombreAtributos;
    vector<short int> tamAtributos;
    vector<short int> tipoAtributo;

    Relacion()
    {
        nombre = "titanic";
        cantAtributos = 12;
        nombreAtributos = {"PassengerId","Survived","Pclass","Name","Sex","Age","SibSp","Parch","Ticket","Fare","Cabin","Embarked"};
        tamAtributos = {4,1,4,50,1,4,4,4,20,15,20,1};
        tipoAtributo = {1,2,1,4,4,1,1,1,4,2,4,4};
    }

    Relacion(string& nombreRelacion) {
        nombre = nombreRelacion;
        string rutaEsquema = "disco\\1\\A\\1\\1.txt";
        /*
        ifstream sector(rutaEsquema + '\\' + to_string(++numSector) + ".txt");


        fstream relacion(nombreRelacion + ".txt", std::ios::app);
        if (!relacion.is_open()) return;
        
        short int pos = 0;
        char letra;

        ifstream esquema("esquema.txt");
        while (esquema >> noskipws >> letra)
        {
            if (letra == '#' && nombreRelacion[pos] == '\0') break;
            else if (letra == nombreRelacion[pos])
            {
                pos++;
                continue;
            }
            else
            {
                while (letra != '\n') esquema >> noskipws >> letra;
                pos = 0;
            }
        }

        std::cout << "\t\t";

        while (esquema >> noskipws >> letra)
        {
            if (letra == '#') break;
        }
        while (esquema >> noskipws >> letra)
        {
            if (letra == '#') break;
        }
        relacion << calcularID(nombreRelacion);

        while (esquema >> noskipws >> letra)
        {
            if (letra == '#')
            {
                std::cout << ": ";
                while (esquema >> noskipws >> letra)
                {
                    if (letra == '#' || letra == '\n') break;
                }

                relacion << '#';
                
                while (std::cin.get(letra))
                {
                    if (letra == '\n') break;
                    relacion << letra;
                }
                std::cout << "\t\t";
            }
            else
            {
                std::cout << letra;
            }
        }
        esquema.close();
        relacion << '\n';
        relacion.close();
        std::cout << "REGISTRO AGREGADO CON EXITO!\n" << endl;
        */
    }
};
Relacion* tabla = nullptr;

class Page
{
public:
    int id;
    vector<int> sectores;

    Page(int id, int sxb)
    {
        this->id = id;
        sectores.resize(sxb);
    }
};

class DiscoVirtual 
{
public:
    short int platos = 0;
    short int pistas = 0;
    short int sectores = 0;
    short int sectorBytes = 0;
    short int sectorxbloque = 0;
    long long capacidad = 0;
    vector<Page*> directorio;

    DiscoVirtual() {}

    void creacionManual()
    {
        std::cout << "> Cantidad de platos: ";
        std::cin >> platos;
        std::cout << "> Cantidad de pistas por superficie: ";
        std::cin >> pistas;
        std::cout << "> Cantidad de sectores por pista: ";
        std::cin >> sectores;
        std::cout << "> Capacidad de un sector(bytes): ";
        std::cin >> sectorBytes;
        std::cout << "> Sectores por bloque: ";
        std::cin >> sectorxbloque;

        long cantSectores = (sectores*pistas*2*platos)/sectorxbloque;
        directorio.resize(cantSectores);
        for (int i = 0; i < cantSectores; i++) directorio[i] = new Page(i+1,sectorxbloque);
        calcularCapacidadTotal();
    }

    void creacionDefault()
    {
        platos = 8;
        pistas = 16;
        sectores = 45;
        sectorBytes = 50;
        sectorxbloque = 5;
        calcularCapacidadTotal();
    }

    int bloqueCapacidad() { return sectorxbloque*sectorBytes; }
    int pistaCapacidad() { return sectores*sectorBytes; }
    long long superficieCapacidad() { return pistas*pistaCapacidad(); }
    long long platoCapacidad() { return 2*superficieCapacidad(); }

    long long calcularCapacidadTotal()
    {
        capacidad = platos*2*pistas*sectores*sectorBytes;
        return capacidad;
    }

    void imprimirCapacidades()
    {
        std::cout << "LA CAPACIDAD DEL DISCO ES... " << capacidad << " BYTES\n";
        std::cout << "LA CAPACIDAD DE UN BLOQUE ES... " << bloqueCapacidad() << " BYTES\n";
    }
    
};
DiscoVirtual* disco;

const short int bytesNombreColumna = 15;
const short int bytesTipoColumna = 10;
const short int bytesTamColumna = 4;

void inicio();
void crearDisco();
void cargarDisco();
void menu();
int sectorDisponible(string&,short int&);
void cargarSectores(string&,short int&,short int&);
void relacion();
void registros();
void consultas();
void logitudFija(string registroInterfaz);
void longitudVariable();


void inicio()
{
    std::cout << "---------------------------------\n";
    std::cout << ">>>> NEO-MEGATRON-3000\n";
    std::cout << "---------------------------------\n";
    std::cout << "Bienvenido a NEO-MEGATRON-3000\n";
    std::cout << endl;

    //ofstream esquema("esquema.txt");
    //esquema.close();
    //crearDisco();
    cargarDisco();
    //relacion();
    //menu();

    std::cout << "Vuelve pronto :D\n";
}

void crearDisco()
{
    std::cout << ">>>> DISCO\n";
    disco = new DiscoVirtual;
	disco->creacionManual();

    string rutaCompleta, rutaBloques;
    int numSector = 0;
    short int numBloque = 0;
    short int cantSectBloque = 0;
    CreateDirectoryW(L"disco", NULL);
    rutaBloques = "disco\\bloques";
    CreateDirectoryW(wstring(rutaBloques.begin(), rutaBloques.end()).c_str(),NULL);
    ofstream bloque;
    ofstream directorio("disco\\directorio.txt");
    for (size_t i = 0; i < disco->platos; i++)
    {
        rutaCompleta = "disco\\" + to_string(i+1);
        CreateDirectoryW(wstring(rutaCompleta.begin(), rutaCompleta.end()).c_str(),NULL);
        rutaCompleta = "disco\\" + to_string(i+1) + "\\A";
        CreateDirectoryW(wstring(rutaCompleta.begin(), rutaCompleta.end()).c_str(),NULL);
        rutaCompleta = "disco\\" + to_string(i+1) + "\\B";
        CreateDirectoryW(wstring(rutaCompleta.begin(), rutaCompleta.end()).c_str(),NULL);
        
        for (size_t j = 0; j < disco->pistas; j++)
        {
            rutaCompleta = "disco\\" + to_string(i+1) + "\\A\\" + to_string(j+1);
            CreateDirectoryW(wstring(rutaCompleta.begin(), rutaCompleta.end()).c_str(),NULL);
            for (size_t k = 0; k < disco->sectores; k++)
            {
                ofstream sector(rutaCompleta + '\\' + to_string(++numSector) + ".txt");
                sector << disco->sectorBytes << endl;
                sector << '#' << endl;
                sector.close();

                if (!bloque.is_open())
                {
                    bloque.open(rutaBloques + '\\' + to_string(++numBloque) + ".txt");
                    directorio << numBloque << ' ';
                }
                bloque << numSector << ' ' << i+1 << " A " << j+1 << endl;
                directorio << numSector;
                if (++cantSectBloque == disco->sectorxbloque)
                {
                    bloque.close();
                    cantSectBloque = 0;
                    directorio << '\n';
                }
                else directorio << ' ';
            }
        }
        
        for (size_t j = 0; j < disco->pistas; j++)
        {
            rutaCompleta = "disco\\" + to_string(i+1) + "\\B\\" + to_string(j+1);
            CreateDirectoryW(wstring(rutaCompleta.begin(), rutaCompleta.end()).c_str(),NULL);
            for (size_t k = 0; k < disco->sectores; k++)
            {
                ofstream sector(rutaCompleta + '\\' + to_string(++numSector) + ".txt");
                sector << disco->sectorBytes << endl;
                sector << '#' << endl;
                sector.close();

                if (!bloque.is_open())
                {
                    bloque.open(rutaBloques + '\\' + to_string(++numBloque) + ".txt");
                    directorio << numBloque << ' ';
                }
                bloque << numSector << ' ' << i+1 << " B " << j+1 << endl;
                directorio << numSector;
                if (++cantSectBloque == disco->sectorxbloque)
                {
                    bloque.close();
                    cantSectBloque = 0;
                    directorio << '\n';
                }
                else directorio << ' ';
            }
        }
    }
    
    if (cantSectBloque != 0) directorio << '\n';
    directorio.close();
    bloque.close();
    
	std::cout << "LA CAPACIDAD DEL DISCO ES... " << disco->capacidad << " BYTES\n";
    //O " << float(disco->capacidad/1000000000) << " GIGAS\n";
    std::cout << "LA CAPACIDAD DE UN BLOQUE ES... " << disco->sectorxbloque*disco->sectorBytes << " BYTES\n"; 

    ofstream metadata("metadata_disco.txt");
    metadata << disco->platos << endl;
    metadata << disco->pistas << endl;
    metadata << disco->sectores << endl;
    metadata << disco->sectorBytes << endl;
    metadata << disco->sectorxbloque << endl;
    metadata.close();
}

void cargarDisco()
{
    char digito;
    short int unidad = 1;
    ifstream metadata("metadata_disco->txt");
    while(metadata >> noskipws >> digito)
    {
        if(digito == '\n')
        {
            metadata >> noskipws >> digito;
            if (metadata.eof()) break;
            unidad++;
        }

        switch (unidad)
        {
        case 1: //Platos
            disco->platos = disco->platos*10 + (digito - '0');
            break;
        case 2: //Pistas
            disco->pistas = disco->pistas*10 + (digito - '0');
            break;
        case 3: //Sectores
            disco->sectores = disco->sectores*10 + (digito - '0');
            break;
        case 4: //Bytes por sector
            disco->sectorBytes = disco->sectorBytes*10 + (digito - '0');
            break;
        case 5:
            disco->sectorxbloque = disco->sectorxbloque*10 + (digito - '0');
            break;
        default:
            break;
        }
    }
    disco->capacidad = disco->platos * 2 * disco->pistas * disco->sectores * disco->sectorBytes;

    std::cout << ">>>> DISCO\n";
    std::cout << "> Cantidad de platos: " << disco->platos << endl;
	std::cout << "> Cantidad de pistas por superficie: " << disco->pistas << endl;
	std::cout << "> Cantidad de sectores por pista: " << disco->sectores << endl;
	std::cout << "> Capacidad de un sector(bytes): " << disco->sectorBytes << endl;
    std::cout << "LA CAPACIDAD DEL DISCO ES... " << disco->capacidad << " BYTES O " << float(disco->capacidad/1000000000) << " GIGAS\n";
    std::cout << "> Sectores por bloque: " << disco->sectorxbloque << endl;
    
    long cantSectores = (disco->sectores*disco->pistas*2*disco->platos)/disco->sectorxbloque;
    (disco->directorio).resize(cantSectores);
    for (int i = 0; i < cantSectores; i++) (disco->directorio)[i] = new Bloque(i+1,disco->sectorxbloque);

    std::cout << "LA CAPACIDAD DE UN BLOQUE ES... " << disco->sectorxbloque*disco->sectorBytes << " BYTES\n";

    tabla = new Relacion();
}

void menu()
{
    char n;
	while(true)
	{
        std::cout << ">>>> MENU\n";
        std::cout << "1. Crear relacion\n";
        std::cout << "2. Agregar Registros\n";
        std::cout << "3. Consulta\n";
        std::cout << "4. Salir\n";
        
        std::cout << "Opcion? ";
        std::cin >> n;
        std::cin.sync();
        std::cout << endl;

        switch (n)
        {
            case '1':
                relacion();
                break;
            case '2':
                registros();
                break;
            case '3':
                consultas();
                break;
            case '4':
                return;
            default:
                std::cout << "Opcion Incorrecta! Intenta otra vez\n" << endl;
                continue;
        }
	}
}

void relacion()
{
    string relacion;
    char letra;
    char tipo;
    short int tam;
    short int cantAtributos = 0;
    short int tamRegistro = 0;

    std::cout << ">>>>>>>> NUEVA RELACION\n";
    std::cout << "Nombre de la relacion: ";
    std::cin >> relacion;
    while (relacion == "")
    {
        std::cout << "Ingrese un nombre valido!\n";
        std::cout << "Nombre de la relacion:";
        std::cin >> relacion;
    }
    tabla = new Relacion(relacion);

    std::cout << "Nro. de atributos: ";
    std::cin >> cantAtributos;
    while (cantAtributos <= 0)
    {
        std::cout << "Ingresa un número válido!\n";
        std::cout << "Nro. de atributos: ";
        std::cin >> cantAtributos;
    }

    string rutaCompleta;
    short int espacio = cantAtributos*(bytesNombreColumna+bytesTipoColumna+bytesTamColumna);
    int numSector = sectorDisponible(rutaCompleta,espacio);
    rutaCompleta = "disco\\1\\A\\1";
    // ofstream sector(rutaCompleta + '\\' + to_string(numSector) + ".txt", std::ios::app);
    ofstream sector(rutaCompleta + '\\' + '1' + ".txt", std::ios::app);
    sector << "esquema_" << relacion << endl;
    sector << '#' << endl;

    string atributo;
    short int tamAtributoColumna = 0;
    for (int i = 0; i < cantAtributos; i++)
    {
        std::cout << "\t>>>> ATRIBUTO " << i+1 << endl;

        tamAtributoColumna = bytesNombreColumna;
        std::cout << "\t> Nombre del atributo: ";
        
        cin >> atributo;
        tamAtributoColumna--;
        sector << atributo;
        tamAtributoColumna -= atributo.size();
        //tabla->nombreAtributos[i] = atributo;

        while (tamAtributoColumna-- != 0) sector << ' ';
        std::cin.sync();

        std::cout << "\t> Tipo:\n";
        std::cout << "\t1. Integer\n";
        std::cout << "\t2. Float\n";
        std::cout << "\t3. Bool\n";
        std::cout << "\t4. Char\n";

        std::cout << "\tOpcion? ";
        std::cin >> tipo;
        std::cin.sync();

        if (tipo == '1' || tipo == '2' || tipo == '3' || tipo == '4')
        {
            if (tipo == '3') tam = 1;
            else
            {
                std::cout << "\t> Tam. Bytes: ";
                std::cin >> tam;
                while (tam <= 0)
                {
                    std::cout << "\tIngrese un tamaño valido!\n";
                    std::cout << "\tTam. Bytes: ";
                    std::cin >> tam;
                }
            }

            switch (tipo)
            {
                case '1':
                    sector << "Integer   ";
                    break;
                case '2':
                    sector << "Float     ";
                    break;
                case '3':
                    sector << "Bool      ";
                    break;
                case '4':
                    sector << "Char      ";
                    break;
                default:
                    sector << "NoType    ";
                    break;
            }
            tamRegistro += tam;
            sector << tam;
            if (tam/10 == 0) sector << "  ";
            else if (tam/100 == 0) sector << " ";

            //tabla->tamAtributos[i] = tam;
            //tabla->tipoAtributo[i] = tipo;
        }
        else std::cout << "\tOpcion Incorrecta! Intenta otra vez\n";
        
        std::cout << endl;
    }

    sector << '\n';
    sector << tamRegistro << endl;
    sector.close();
    std::cout << "\tCAPACIDAD DE UN REGISTRO: " << tamRegistro << endl;
    short int registroSector = disco->sectorBytes/tamRegistro;
    std::cout << "\tREGISTROS POR SECTOR: " << disco->sectorBytes/tamRegistro << endl;
    cargarSectores(relacion,registroSector,tamRegistro);
}

void cargarRelacion()
{
    string rutaCompleta = "disco\\1\\A\\1";
    ifstream sector( "disco\\1\\A\\1\\1.txt", std::ios::in);
    char letra;
    string palabra = "";

    for (int i = 0; i < 4; i++)
    {
        while(sector >> noskipws >> letra)
        {
            if (letra == '\n') break;
        }
    }

    while(sector >> noskipws >> letra)
    {
        while (letra == ' ') sector >> noskipws >> letra;
        if (letra == '\n') break;
    }  
}

void cargarSectores(string& relacion, short int& registroSector, short int& tamRegistro)
{
    return;
}

int sectorDisponible(string& rutaCompleta, short int& espacio)
{
    int numSector = 0;
    int disponible = 0;
    char digito;
    for (size_t i = 0; i < disco->platos; i++)
    {
        for (size_t j = 0; j < disco->pistas; j++)
        {
            for (size_t k = 0; k < disco->sectores; k++)
            {
                //if (i == 0 && j == 0 && k == 0) continue;
                numSector = i*2*disco->pistas*disco->sectores + j*disco->sectores + k+1;
                rutaCompleta = "disco\\" + to_string(i+1) + "\\A\\" + to_string(j+1);
                ifstream sector(rutaCompleta + '\\' + to_string(numSector) + ".txt");
                while(sector >> noskipws >> digito)
                {
                    if (digito == '\n') break;
                    disponible = disponible*10 + (digito - '0');
                }
                sector.close();
                if (disponible >= espacio)
                {
                    short int actualizacion = disponible - espacio;
                    ofstream nuevaCapacidad(rutaCompleta + '\\' + to_string(numSector) + ".txt", std::ios::in);
                    if (nuevaCapacidad.is_open()) std::cout << "SI!";
                    std::cout << actualizacion << " ";
                    std::cout << disponible;
                    nuevaCapacidad << actualizacion;
                    while (disponible != 0)
                    {
                        disponible = disponible/10;
                        actualizacion = actualizacion/10;
                        std::cout << disponible << " " << actualizacion << endl;
                        if(actualizacion == 0 && disponible > 0) nuevaCapacidad << ' ';
                    }
                    nuevaCapacidad.close();
                    return numSector;
                }
                disponible = 0;
            }
        }
        
        for (size_t j = 0; j < disco->pistas; j++)
        {
            
            CreateDirectoryW(wstring(rutaCompleta.begin(), rutaCompleta.end()).c_str(),NULL);
            for (size_t k = 0; k < disco->sectores; k++)
            {
                numSector = i*2*disco->pistas*disco->sectores + disco->pistas*disco->sectores + j*disco->sectores + k+1;
                rutaCompleta = "disco\\" + to_string(i+1) + "\\A\\" + to_string(j+1);
                ifstream sector(rutaCompleta + '\\' + to_string(numSector) + ".txt");
                while(sector >> noskipws >> digito)
                {
                    if (digito == '\n') break;
                    disponible = disponible*10 + (digito - '0');
                }
                sector.close();
                if (disponible >= espacio) return numSector;
                disponible = 0;
            }
        }
    }
    return numSector;
}

short int calcularID(string& nombreRelacion)
{
    short int ID = 0;
    char letra;

    ifstream relacion(nombreRelacion + ".txt", std::ios::app);

    while (relacion >> noskipws >> letra)
    {
        if (letra ==  '\n') ID++;
    }
    relacion.close();
    ID++;
    return ID; 
}

void escribirRegistro(string& nombreRelacion)
{
    fstream relacion(nombreRelacion + ".txt", std::ios::app);
    if (!relacion.is_open()) return;
    
    short int pos = 0;
    char letra;

    ifstream esquema("esquema.txt");
    while (esquema >> noskipws >> letra)
    {
        if (letra == '#' && nombreRelacion[pos] == '\0') break;
        else if (letra == nombreRelacion[pos])
        {
            pos++;
            continue;
        }
        else
        {
            while (letra != '\n') esquema >> noskipws >> letra;
            pos = 0;
        }
    }

    std::cout << "\t\t";

    while (esquema >> noskipws >> letra)
    {
        if (letra == '#') break;
    }
    while (esquema >> noskipws >> letra)
    {
        if (letra == '#') break;
    }
    relacion << calcularID(nombreRelacion);

    while (esquema >> noskipws >> letra)
    {
        if (letra == '#')
        {
            std::cout << ": ";
            while (esquema >> noskipws >> letra)
            {
                if (letra == '#' || letra == '\n') break;
            }

            relacion << '#';
            
            while (std::cin.get(letra))
            {
                if (letra == '\n') break;
                relacion << letra;
            }
            std::cout << "\t\t";
        }
        else
        {
            std::cout << letra;
        }
    }
    esquema.close();
    relacion << '\n';
    relacion.close();
    std::cout << "REGISTRO AGREGADO CON EXITO!\n" << endl;
}

void cargarRegistros()
{
    string rutaCompleta;
    for (size_t j = 0; j < disco->pistas; j++)
    {
        rutaCompleta = "disco\\" + to_string(i+1) + "\\B\\" + to_string(j+1);
        CreateDirectoryW(wstring(rutaCompleta.begin(), rutaCompleta.end()).c_str(),NULL);
        for (size_t k = 0; k < disco->sectores; k++)
        {
            ofstream sector(rutaCompleta + '\\' + to_string(++numSector) + ".txt");
            sector << disco->sectorBytes << endl;
            sector << '#' << endl;
            sector.close();

            if (!bloque.is_open())
            {
                bloque.open(rutaBloques + '\\' + to_string(++numBloque) + ".txt");
                directorio << numBloque << ' ';
            }
            bloque << numSector << ' ' << i+1 << " B " << j+1 << endl;
            directorio << numSector;
            if (++cantSectBloque == disco->sectorxbloque)
            {
                bloque.close();
                cantSectBloque = 0;
                directorio << '\n';
            }
            else directorio << ' ';
        }
    }
}

void registros()
{
    string nombreRelacion = "titanic";
    char n;
    std::cout << ">>>>>>>> AGREGAR REGISTROS\n";

    /*
    while (true)
    {
        std::cout << "Relacion: ";
        std::cin >> nombreRelacion;
        fstream relacion(nombreRelacion + ".txt", std::ios::app);
        if (relacion.is_open())
        {
            relacion.close();
            break;
        }
        else std::cout << "Relacion no encontrada! Ingrese una relacion valida\n";
    }
    */
    
    while(true)
	{
        std::cout << "\t>>>> TIPO DE AGREGACION\n";
        std::cout << "\t1. Escribir registro\n";
        std::cout << "\t2. Cargar registros\n";
        
        std::cout << "\tOpcion? ";
        std::cin >> n;
        std::cin.sync();

        switch (n)
        {
            case '1':
                escribirRegistro(nombreRelacion);
                break;
            case '2':
                cargarRegistros(nombreRelacion);
                break;
            default:
                std::cout << "Opcion Incorrecta! Intenta otra vez\n" << endl;
                break;
        }
        if (n == '1' || n == '2') break;
	}
}

void select()
{
    char letra = ' ';
    short int pos = 0;
    char atributo[30];
    char relacion[30];
    char columna[30];
    short int tipo = 0;
    char condicion[30];
    const char from[] = "FROM";
    const char where[] = "WHERE";
    
    //Primer espacio
    while (letra == ' ') std::cin.get(letra);
    
    //Reconoce atributo
    while (letra != ' ')
    {
        atributo[pos] = letra;
        pos++;
        std::cin.get(letra);
    }
    atributo[pos] = '\0';
    pos = 0;

    //Segundo espacio
    while (letra == ' ') std::cin.get(letra);

    //Reconoce FROM
    do
    {
        if (letra == from[pos])
        {
            if (from[pos] == '\0') break;
            pos++;
        }
        else pos = 0;
    }
    while (std::cin.get(letra));
    pos = 0;

    //Tercer espacio
    while (letra == ' ') std::cin.get(letra);

    //Reconoce relacion
    while (letra != ' ' && letra != '\n')
    {
        relacion[pos] = letra;
        pos++;
        std::cin.get(letra);
    }
    pos = 0;

    if (letra == '\n')
    {
        if (atributo[0] == '*')
        {
            ifstream esquema("esquema.txt");
            while (esquema >> noskipws >> letra)
            {
                if (letra == '#') break;
                else if (letra == relacion[pos])
                {
                    pos++;
                    continue;
                }
                else
                {
                    while (letra != '\n') esquema >> noskipws >> letra;
                    pos = 0;
                }
            }
            
            while (esquema >> noskipws >> letra)
            {
                if (letra == '#') std::cout << "\t\t";
                else
                {
                    std::cout << letra;
                    if (letra == '\n') break;
                }
            }
            esquema.close();

            const char extension[] = ".txt";
            //strcat(relacion, extension);
            ifstream archivo(relacion);
            while (archivo >> noskipws >> letra)
            {
                if (letra == '#') std::cout << "\t\t";
                else std::cout << letra;
            }
            archivo.close();
        }
        else
        {
            // short int numAtributo = 0;
            // ifstream esquema("esquema.txt");
            // while (esquema >> noskipws >> letra)
            // {
            //     if (letra == '#') break;
            //     else if (letra == relacion[pos])
            //     {
            //         pos++;
            //         continue;
            //     }
            //     else
            //     {
            //         while (letra != '\n') esquema >> noskipws >> letra;
            //         pos = 0;
            //     }
            // }
            // pos = 0;

            // while (esquema >> noskipws >> letra)
            // {
            //     if (letra == '#') break;
            //     else if (letra == atributo[pos])
            //     {
            //         pos++;
            //         continue;
            //     }
            //     else
            //     {
            //         while (letra != '#')
            //         {
            //             esquema >> noskipws >> letra;
            //             numAtributo++;
            //         }
            //         pos = 0;
            //     }
            // }
            // esquema.close();

            // short int step = 0;

            // const char extension[] = ".txt";
            // strcat(relacion, extension);
            // ifstream archivo(relacion);
            // std::cout << atributo << endl;
            // while (step < numAtributo)
            // {
            //     archivo >> noskipws >> letra;
            //     if (letra == '#') step++;
            // }
            // archivo.close();
        }
    }
    else
    {
        //Cuarto espacio
        while (letra == ' ') std::cin.get(letra);

        //Reconoce WHERE
        do
        {
            if (letra == where[pos])
            {
                if (from[pos] == '\0')
                {
                    break;
                }
                pos++;
            }
            else pos = 0;
        }
        while (std::cin.get(letra));
    }
}

void consultas()
{
    std::cout << ">>>>>>>> CONSULTA\n";
    std::cout << "Ingrese su consulta:\n";
    std::cout << "> ";

    short int pos = 0;
    const char palabra[] = "SELECT";
    char letra;

    while (std::cin.get(letra))
    {
        if (letra == palabra[pos])
        {
            if (palabra[pos] == '\0')
            {
                select();
                break;
            }
            pos++;
        }
        else pos = 0;
    }
}

int main()
{
    inicio();
    return 0;
}