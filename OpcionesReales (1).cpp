#include <iostream>
#include <vector>
#include <stack>
#include <time.h>
#include <math.h>

using namespace std;

template<typename T>
class InversionRO{
	struct Nodo{
		T monto;		
		T inversion;
		T posibilidad;
		Nodo* gana;
		Nodo* pierde;
		
		Nodo(T inversion = 0, T posibilidad = 1, T monto = 0, Nodo *gana = nullptr, Nodo *pierde = nullptr) : monto(monto), inversion(inversion), posibilidad(posibilidad), gana(gana), pierde(pierde) {}
	};
	
	int n;
	
	Nodo* inicio;
	
	
	//Se genera la inversi�n que dar�a ganancias
	//En cada a�o siguiente, la nueva inversi�n necesaria ser�a modificada por la insertidumbre
	//Asimismo, la insertidumbr ir�a variando con iguales probabilidades de aumentar o disminuir
	void AgregarInversionGanadora(Nodo*& nodo, T p, T monto, int actual, T inversion, T incertidumbre, T StandarDesviation){
		cout<<"Inversion Buena "<<actual<<") ";
		if(actual < n){
			inversion = inversion * incertidumbre;
			if(incertidumbre - StandarDesviation <= 0){
				incertidumbre+= StandarDesviation;
			}else if(incertidumbre + StandarDesviation >= 2){
				incertidumbre-= StandarDesviation;
			}else{
				int r = rand()%2;
				if(r == 0){
					incertidumbre = incertidumbre - StandarDesviation;
				}else{
					incertidumbre = incertidumbre + StandarDesviation;
				}				
			}
			++actual;
			nodo = new Nodo(inversion, p, monto+inversion);
			p = (rand()%100+1)/100;
			cout<<nodo->inversion<<" || "<<nodo->posibilidad<<" || "<<nodo->monto<<endl;
			AgregarInversionGanadora(nodo->gana, p, nodo->monto, actual, nodo->inversion, incertidumbre, StandarDesviation);
			AgregarInversionPerdedora(nodo->pierde, 1-p, nodo->monto, actual, nodo->inversion, incertidumbre, StandarDesviation);
		}else{
			//En caso de haber llegado al a�o final estimado, se estima la remuneraci�n total obetinada
			nodo = new Nodo(0, p, monto + (inversion * incertidumbre * n));
			cout<<"Monto : "<<nodo->monto<<"\t Posibilidad : "<<nodo->posibilidad<<endl;
			cout<<"FIN"<<endl;
		}
	}
	
	//En este caso se calcular�a la "p�rdida" o ganancia m�nima
	//En cada a�o siguiente, la nueva inversi�n necesaria ser�a modificada por la insertidumbre
	//Asimismo, la insertidumbr ir�a variando con iguales probabilidades de aumentar o disminuir
	void AgregarInversionPerdedora(Nodo*& nodo, T p, T monto, int actual, T inversion, T incertidumbre, T StandarDesviation){
		cout<<"Inversion Mala "<<actual<<") ";
		if(actual < n){
			++actual;
			inversion = inversion * incertidumbre;
			if(incertidumbre - StandarDesviation < 0){
				incertidumbre+= StandarDesviation;
			}else if(incertidumbre + StandarDesviation > 2){
				incertidumbre-= StandarDesviation;
			}else{
				int r = rand()%2;
				if(r == 0){
					incertidumbre = incertidumbre - StandarDesviation;
				}else{
					incertidumbre = incertidumbre + StandarDesviation;
				}				
			}
			nodo = new Nodo(inversion, p, monto - inversion);
			p = (rand()%100+1)/100;
			cout<<nodo->inversion<<" || "<<nodo->posibilidad<<" || "<<nodo->monto<<endl;
			AgregarInversionGanadora(nodo->gana, p, nodo->monto, actual, nodo->inversion, incertidumbre, StandarDesviation);
			AgregarInversionPerdedora(nodo->pierde, 1-p, nodo->monto, actual, nodo->inversion, incertidumbre, StandarDesviation);
		}else{
			//En caso de haber llegado al a�o final estimado, se estima la remuneraci�n total obetinada
			nodo = new Nodo(0, p, monto - (inversion * incertidumbre * n));
			cout<<"Monto PErd: "<<nodo->monto<<"\t Posibilidad Perd: "<<nodo->posibilidad<<endl;
			cout<<"FIN PERDEDOR"<<endl;
		}
	}
	
	//Se va recolectando valores de las inversiones y sus probabilidades de �xito para poder hallar el NPV
	float ReturnOutcomeValue(Nodo*& nodo){
		if(nodo->gana->gana == nullptr){
			//Si el siguiente a�o se dejar� de invertir, se adquiere ambos valores de inversi�n (la �ptima y la no �ptima)
			return nodo->gana->posibilidad * nodo->gana->monto  + nodo->pierde->posibilidad * nodo->pierde->monto;
		}else{
			//En caso de que se halla seguido invirtiendo en una propuesta ganadaora, se sigue extrayendo valores del siguiente a�o
			float aux = nodo->gana->posibilidad * nodo->gana->monto + ReturnOutcomeValue(nodo->gana);
			return aux;
		}
	}
	
public:
	InversionRO(): n(0), inicio(nullptr){}
	~InversionRO(){
		stack<Nodo*> pilita;
		pilita.push(inicio);
		while (pilita.size() > 0){
			Nodo* nodo = pilita.top();
			pilita.pop();
			if(nodo != nullptr){
				pilita.push(nodo->gana);
				pilita.push(nodo->pierde);
				delete nodo;
			}
		}
	}
	
	//Se agrega la primera inversi�n
	void AgregarPrimeraInversion(T inversion, T monto){
		inicio = new Nodo(inversion, 1, monto);
	}
	
	
	//Se empieza a generar las futuras inversiones y las �ltimas remuneraciones al final del tiempo estimado
	void GenerarRealOptions(int tiempo, T incertidumbre, T StandarDesviation){
		n = tiempo;
		float p = (rand()%99+1)/100 ;
		cout<<"\n"<<endl;
		AgregarInversionGanadora(inicio->gana, p, inicio->monto, 0, inicio->inversion, incertidumbre, StandarDesviation);
		cout<<"\n"<<endl;
		AgregarInversionPerdedora(inicio->pierde, 1-p, inicio->monto, 0, inicio->inversion, incertidumbre, StandarDesviation);
		cout<<"\n"<<endl;
	}
	
	//Se adquiere el NPV
	float GetNPV(){
		cout<<"\nGenerar NPV: ";
		float NPV = 0;
		NPV = - inicio->inversion - (inicio->gana->inversion / 1.12) + ReturnOutcomeValue(inicio->gana) / pow(1.12f,float(n));
		return NPV;
	}
};


int main(){
	srand(time(0));
	float inversion, incertidumbre, StandarDesviation, montoInicial;
	int tiempo;
	//inversion = 1500; incertidumbre = StandarDesviation = 0.5f; tiempo = 3;

	//Se pide ingresar el monto con el que la empresa cuenta
	cout<<"Ingrese monto inicial en millones con que se cuenta: "; cin>> montoInicial;
	
	//Se pide la primera inversi�n que se financiar�a
	cout<<"Ingrese inversion inicial en millones: "; cin>>inversion;
	
	//Se pide la incertidumbre de la inversi�n conjunta
	do{
		cout<<"Ingrese incertidumbre en porcentaje (0 a 1): ";
		cin>>incertidumbre;
	}while(incertidumbre < 0 || incertidumbre > 1);
	
	//El tiempo se�alar� qu� tan ramificado ser�a la inversi�n
	do{
		cout<<"Ingrese tiempo de expiracion en a�os (2 a 12): ";
		cin>>tiempo;
	}while(tiempo < 2 || tiempo >12);
	
	//La desviaci�n est�ndar permitir� que por cada a�o, la nueva inversi�n tenga una nueva probabilidad de que sea buena o no
	do{
		cout<<"Ingrese desviacion estandar por a�o (0 a 1): ";
		cin>>StandarDesviation;
	}while(StandarDesviation < 0 || StandarDesviation > 1);
	
	
	//Generaci�n del �rbol booleano para Real Options
	InversionRO<float>* grafico = new InversionRO<float>();
	grafico->AgregarPrimeraInversion(inversion, montoInicial);
	grafico->GenerarRealOptions(tiempo, incertidumbre, StandarDesviation);
	cout<<grafico->GetNPV();
	return 0;
}
