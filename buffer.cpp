#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <iomanip>

using namespace std;

class Page;
class Frame;
class BufferManager;
BufferManager* bm;
vector<Page*> paginas;

void menu();

class Page {
public:
    int id;
    Page(int id) : id(id) {}
    void save() {} 
};

class Frame {
    private:
        int id;
        Page* pagina;
        bool dirtyFlag;
        int pinCount;
        int lastUsed;
        bool pinned;
        bool refBit;
        queue<bool> requirements;
        int writeRequirements;
    public:
        Frame(int i): dirtyFlag(0), pinCount(0), lastUsed(0), pinned(false), refBit(false), writeRequirements(0){
            this->id = i;
            pagina = nullptr;
        }

        int getId(){ return id; }
        
        int getPinCount(){ return pinCount; }
        
        void resetFrame() {
            pagina = nullptr;
            dirtyFlag = false;
            pinCount = 0;
            lastUsed = 0;
        }

        Page* getPagina(){  return pagina;  }
        
        void setPage(Page *pag){ this->pagina = pag; }
        
        void incrementPinCount(){ pinCount++; }
        
        void decrementPinCount(){
            if (pinCount > 0) pinCount--;
        }
        
        void incrementLastUsed(){ lastUsed++; }

        int getLastUsed(){ return lastUsed; }

        void setLastUsed(int lused){  
            this->lastUsed = lused;    
        }
        
        void setDirty(bool accion){ dirtyFlag = accion; }
        
        bool getDirty(){ return dirtyFlag; }
        void changeDirty(){ dirtyFlag = !dirtyFlag; }

        void saveChanges(){ pagina->save(); }

        bool getRefBit(){ return refBit; }
        void changeRefBit(){ refBit = !refBit; }

        bool getPinned(){ return pinned; }
        void changePinned(){ pinned = !pinned; }

        void addRequirement(bool accion){
            requirements.push(accion);
            if (accion)
            {
                writeRequirements++;
                dirtyFlag = true;
            }
            refBit = true;
            lastUsed = 0;
            pinCount++;
            cout << "\t* Requerimiento agregado\n";
        }
        void releaseRequirement()
        {
            if (requirements.front() == true)
            {
                cout << "\tDeseas guardar los cambios de la pagina " << pagina->id << "? (SI = 1, NO = 0): ";
                bool opcion;
                cin >> opcion;
                if (opcion)
                {
                    pagina->save();
                    cout << "\t* La pagina ha sido guardada correctamente\n";
                }
                else cout << "\t* Cambios descartados\n";
                writeRequirements--;
            }
            requirements.pop();
            cout << "\t* Requerimiento finalizado\n";
            lastUsed = 0;
            pinCount--;
            if (writeRequirements == 0) dirtyFlag = 0;
        }
        bool getCurrentRequirement(){ return requirements.front(); }
        bool isEmptyRequirements(){ return requirements.empty(); }
};

class BufferManager {
    private:
        map<int, int> pageTable;
        vector<Frame*> bufferPool;
        int hits;
        int solicitudes;
        int clockHand = 0;
        int framesDisponibles;
        short int estrategia; // 0=LRU, 1=MRU, 2=RELOJ
    public:
        /*
        Constructor por defecto
            poolSize = indica el numero de frames   
        */
        BufferManager() : hits(0), solicitudes(0) {
            short int poolSize = 4;
            framesDisponibles = poolSize;
            bufferPool.resize(poolSize);
            for (int i = 0; i < poolSize; i++) {
                bufferPool[i] = new Frame(i);
            }
            estrategia = 2;
        }

        /*
        Constructor con parametros
        */
        BufferManager(short int poolSize, short int est) : hits(0), solicitudes(0) {
            framesDisponibles = poolSize;
            bufferPool.resize(poolSize);
            for (int i = 0; i < poolSize; i++) {
                bufferPool[i] = new Frame(i);
            }
            estrategia = est;
        }

        ~BufferManager() {
            for (auto& frame : bufferPool) {
                delete frame;
            }
        }

        /*
        IMprime los valores del page table (id pagina y id frame)
        */
        void printPageTable(){
            cout << "\t> Page table\n"<<endl;
            cout << "page_id  frame_id"<< endl;
            for (const auto &entry: pageTable) {
                cout << "{" << entry.first << ", " << entry.second << "}" << endl;
            }
        }

        /*
        Incrementa todos los last used de todos los frames
        */
        void aumentarLastUsed(){
            for (auto& frame: bufferPool){
                if(!frame->getPinned()) frame->incrementLastUsed();
            }
        }

        /*
        Se imprime informacion del frame:
            frame_id, page_id, mode (dirty bit), pinCount, last_used    
        */
        void printBufferPool(){
            cout << "\t> Buffer Pool\n";
            cout << setfill(' ');
            cout << setfill('-') << setw(12) << "+-" 
                << setfill('-') << setw(12) << "+-";
            if (estrategia == 2) cout << setfill('-') << setw(12) << "+-";
            cout << setfill('-') << setw(12) << "+-"
                << setfill('-') << setw(12) << "+-"
                << setfill('-') << setw(17) << "+-"
                << setfill('-') << setw(12) << "+-"
                << setfill('-') << setw(12) << "+-"
                << "+\n" << setfill(' ');
            cout << "| " << left << setw(10) << "Frame Id"
                << "| " << left << setw(10) << "Page Id";
            if (estrategia == 2) cout << "| " << left << setw(10) << "Ref Bit";
            cout << "| " << left << setw(10) << "Pin Count"
                << "| " << left << setw(10) << "Dirty Bit"
                << "| " << left << setw(15) << "Requirement"
                << "| " << left << setw(10) << "Pinned"
                << "| " << left << setw(10) << "Last Used"
                << "|" << endl;
            cout << setfill('-') << setw(12) << "+-" 
                << setfill('-') << setw(12) << "+-";
            if (estrategia == 2) cout << setfill('-') << setw(12) << "+-";
            cout << setfill('-') << setw(12) << "+-"
                << setfill('-') << setw(12) << "+-"
                << setfill('-') << setw(17) << "+-"
                << setfill('-') << setw(12) << "+-"
                << setfill('-') << setw(12) << "+-"
                << "+\n" << setfill(' ');
            for (auto& frame: bufferPool){
                cout << "| " << left << setw(10) << frame->getId();
                if (frame->getPagina())
                {
                    cout << "| " << left << setw(10) << frame->getPagina()->id;
                    if (estrategia == 2) cout << "| " << left << setw(10) << (frame->getRefBit()? '1' : '0');
                    cout << "| " << left << setw(10) << frame->getPinCount()
                         << "| " << left << setw(10) << (frame->getDirty()? '1' : '0')
                         << "| " << left << setw(15) << (frame->isEmptyRequirements()? '-' : (frame->getCurrentRequirement()? 'W' : 'R'))
                         << "| " << left << setw(10) << (frame->getPinned()? '1' : '0')
                         << "| " << left << setw(10) << frame->getLastUsed()
                         << "|" << endl;
                }
                else
                {
                    cout << "| " << left << setw(10) << "-";
                    if (estrategia == 2) cout << "| " << left << setw(10) << (frame->getRefBit()? '1' : '0');
                    cout << "| " << left << setw(10) << frame->getPinCount()
                         << "| " << left << setw(10) << "-"
                         << "| " << left << setw(15) << "-"
                         << "| " << left << setw(10) << "-"
                         << "| " << left << setw(10) << "-"
                         << "|" << endl;
                }
            }
            cout << setfill('-') << setw(12) << "+-" 
                << setfill('-') << setw(12) << "+-";
            if (estrategia == 2) cout << setfill('-') << setw(12) << "+-";
            cout << setfill('-') << setw(12) << "+-"
                << setfill('-') << setw(12) << "+-"
                << setfill('-') << setw(17) << "+-"
                << setfill('-') << setw(12) << "+-"
                << setfill('-') << setw(12) << "+-"
                << "+\n" << setfill(' ');
            cout << setfill(' ');
        }

        float hitRate() { return (float)hits/solicitudes; }
        bool existPage(int pageId) { return pageTable.find(pageId) != pageTable.end(); }
        /*
        Maneja el ingreso de paginas, busca si la página está en el Buffer Pool
            Si esta, incrementa el hits, modifica el last used y ve en que modo esta (escritura o lectura)
            Si no está, encontra un frame libre y anexar la página ahi
        */
        void pinPage(int pageId, Page* page, bool accion){
            Frame* framePage = nullptr;
            if (existPage(pageId)) {
                hits++;
                framePage = bufferPool[pageTable[pageId]];
                framePage->addRequirement(accion);
            }
            else {
                if(!newPage(pageId, page, accion)) return;
            }
            framePage = bufferPool[pageTable[pageId]];
            aumentarLastUsed();

            solicitudes++;
            cout << endl;
            cout << "\t> INFORMACION DE SOLICITUD\n";
            cout << "\tNro de solicitud: "<< solicitudes << endl;
            cout << "\tMisses: " << solicitudes-hits << endl;
            cout << "\tHits: " << hits << endl;
        }

        /*
        Maneja la operacion de 'despinear' una pagina de acuerdo a su id, decrementando
        su pincount (indicando que se estan disminuyendo accesos a esa pagina)
        */
        void unpinPage(int pageId)
        {
            Frame* framePage = nullptr;
            if (existPage(pageId))
            {
                framePage = bufferPool[pageTable[pageId]];
                if(framePage->getPinCount() > 0)
                {
                    framePage->releaseRequirement();
                    if (framePage->getPinCount() == 0 && !framePage->getPinned()) framesDisponibles++;
                    aumentarLastUsed();
                }
                else
                {
                    cout << "\t* La pagina no esta siendo usada por ningun requerimiento\n";
                    if(framePage->getPinned())
                    {
                        cout << "\t* La pagina esta pinneada, se necesita despinnear para reemplazarla\n";
                    }
                }
            }
            else cout << "\t* La pagina a liberar no se encuentra en el buffer\n";
        }
        
        bool newPage(int pageId, Page* page, bool accion){
            Frame* frameActual = nullptr;
            
            if (framesDisponibles == 0)
            {
                cout << "\t* No hay ningun frame disponible\n";
                return false;
            }
            //cout << "\tFrames disponibles: " << framesDisponibles << endl;
            short int numFrameReemplazo;
            short int maxLastUsed = -1;
            short int minLastUsed = solicitudes+1;

            if (estrategia == 0) //LRU
            {
                for (auto& frame: bufferPool)
                {
                    if (frame->getPinCount() == 0 && !frame->getPinned())
                    {
                        if (frame->getLastUsed() > maxLastUsed)
                        {
                            maxLastUsed = frame->getLastUsed();
                            numFrameReemplazo = frame->getId();
                        }
                    }
                }
            }
            else if (estrategia == 1) //MRU
            {
                for (auto& frame: bufferPool)
                {
                    if (frame->getPinCount() == 0 && !frame->getPinned())
                    {
                        if (frame->getLastUsed() < minLastUsed)
                        {
                            minLastUsed = frame->getLastUsed();
                            numFrameReemplazo = frame->getId();
                        }
                    }
                }
            }
            else if (estrategia == 2) //RELOJ
            {
                while(true)
                {
                    frameActual = bufferPool[clockHand];
                    clockHand++;
                    clockHand = clockHand%bufferPool.size();

                    if (frameActual->getPinCount() == 0 && !frameActual->getPinned())
                    {
                        frameActual->changeRefBit();
                        if(frameActual->getRefBit())
                        {
                            numFrameReemplazo = frameActual->getId();
                            break;
                        }
                    }
                }
            }
            
            frameActual = bufferPool[numFrameReemplazo];
            if(frameActual->getPagina()) pageTable.erase(frameActual->getPagina()->id);
            frameActual->resetFrame();
            frameActual->setPage(page);
            frameActual->addRequirement(accion);
            pageTable[pageId] = frameActual->getId(); //Ingresar a la tabla
            framesDisponibles--;
            return true;
        }

        void pinnedPage(int pageId)
        {
            Frame* framePage = nullptr;
            if (existPage(pageId))
            {
                framePage = bufferPool[pageTable[pageId]];

                if (!framePage->getPinned())
                {
                    framePage->changePinned();
                    if(framePage->getPinCount() == 0) framesDisponibles--;
                    cout << "\t* Pagina pinneada con exito!\n";
                }
                else cout << "\t* La pagina ya se encontraba pinneada\n";
            }
            else cout << "\t* La pagina a pinnear no se encuentra en el buffer\n";
        }

        void unpinnedPage(int pageId)
        {
            Frame* framePage = nullptr;
            if (existPage(pageId))
            {
                framePage = bufferPool[pageTable[pageId]];

                if (framePage->getPinned())
                {
                    framePage->changePinned();
                    if(framePage->getPinCount() == 0) framesDisponibles++;
                    cout << "\t* Pagina despinneada con exito!\n";
                }
                else cout << "\t* La pagina ya se encontraba despinneada\n";
            }
            else cout << "\t* La pagina a despinnear no se encuentra en el buffer\n";
        }
};

void solicitarPagina(){
    cout << "\t> SOLICITUD\n";
    int idPagina;
    char accion;
    do{
        cout << "\tID de la pagina: ";
        cin >> idPagina;
    } while (idPagina > 100 || idPagina <= 0);
    
    cout << "\tLectura (R) o escritura (W)?: ";
    cin >> accion;
    if (accion == 'R'){
        accion = false;
    } else if (accion == 'W'){
        accion = true;
    }
    bm->pinPage(idPagina, paginas[idPagina-1], accion);
}

void liberarPagina()
{
    cout << "\t> LIBERACION\n";
    int idPagina;
    do{
        cout << "\tID de la pagina: ";
        cin >> idPagina;
    } while (idPagina > 100 || idPagina <= 0);

    bm->unpinPage(idPagina);
}

void pinnearPagina()
{
    cout << "\t> PINNED\n";
    int idPagina;
    do{
        cout << "\tID de la pagina: ";
        cin >> idPagina;
    } while (idPagina > 100 || idPagina <= 0);

    bm->pinnedPage(idPagina);
}

void despinnearPagina()
{
    cout << "\t> UNPINNED\n";
    int idPagina;
    do{
        cout << "\tID de la pagina: ";
        cin >> idPagina;
    } while (idPagina > 100 || idPagina <= 0);

    bm->unpinnedPage(idPagina);
}

void menu()
{
    char opcion;
    cout << "-------------------------------------------------------------------------------------------------------\n";
    cout << ">>>> BUFFER MANAGER\n";
    cout << "-------------------------------------------------------------------------------------------------------\n";

    short int numFrames, estrategia;
    cout << "Numero de frames: ";
    cin >> numFrames;
    cout << "Estrategia de reemplazo (LRU = 0, MRU = 1, RELOJ = 2): ";
    cin >> estrategia;
    bm = new BufferManager(numFrames, estrategia);

    do {
        cout << endl;
        cout << "=======================================================================================================\n";
        cout << ">> OPTIONS\n";
        cout << "1. Solicitar pagina\n";
        cout << "2. Liberar pagina\n";
        cout << "3. Imprimir Buffer Pool\n";
        cout << "4. Imprimir Hit Rate\n";
        cout << "5. Pinnear pagina\n";
        cout << "6. Despinnear pagina\n";
        cout << "7. Salir\n";
        cout << "Opcion? ";
        cin >> opcion;
        
        switch(opcion)
        {
            case '1':
                solicitarPagina();
                break;
            case '2':
                liberarPagina();
                break;
            case '3':
                bm->printBufferPool();
                break;
            case '4':
                cout << "\t> HIT RATE: " << bm->hitRate() << endl;
                break;
            case '5':
                pinnearPagina();
                break;
            case '6':
                despinnearPagina();
                break;
            case '7':
                cout << "\t> ADIOS!\n";
                break;
            default:
                break;
        }
    } while (opcion != '7');
}

int main (){
    
    paginas.resize(100);
    for (int i = 0; i < 100; i++) {
        paginas[i] = new Page(i+1);
    }
    menu();

    return 0;
}