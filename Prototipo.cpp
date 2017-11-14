#include <iostream>
#include <vector>
#include <stack>
#include <time.h>
#include <math.h>

using namespace std;

template<typename T>
class InversionRO{
	struct Nodo{
		T e;
		Nodo* gana;
		Nodo* pierde;
		T Incertidumbre;
		
		Nodo(T e = 0, T Incertidumbre = 1, Nodo *gana = nullptr, Nodo *pierde = nullptr) : e(e), Incertidumbre(Incertidumbre), gana(gana), pierde(pierde) {}
	};
	
	int n;
	
	Nodo* inicio;
	
	//Se genera la inversi�n que dar�a ganancias
	//En cada a�o siguiente, la nueva inversi�n necesaria ser�a modificada por la insertidumbre
	//Asimismo, la insertidumbr ir�a variando con iguales probabilidades de aumentar o disminuir
	void AgregarInversionGanadora(Nodo*& nodo, int tiempo, int actual, T e, T incertidumbre, T StandarDesviation){
		cout<<"Inversion Buena "<<actual<<") ";
		if(actual < tiempo){
			++actual;
			nodo = new Nodo(e, incertidumbre);
			e = e * incertidumbre;
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
			cout<<nodo->e<<" || "<<nodo->Incertidumbre<<endl;
			AgregarInversionGanadora(nodo->gana, tiempo, actual, e, incertidumbre, StandarDesviation);
			AgregarInversionPerdedora(nodo->pierde, tiempo, actual, e, incertidumbre, StandarDesviation);
		}else{
			//En caso de haber llegado al a�o final estimado, se estima la remuneraci�n total obetinada
			nodo = new Nodo(e* incertidumbre * tiempo, incertidumbre);
			cout<<"Monto: "<<nodo->e<<"\t Incertidumbre: "<<nodo->Incertidumbre<<endl;
			cout<<"FIN"<<endl;
		}
	}
	
	//En este caso se calcular�a la "p�rdida" o ganancia m�nima
	//En cada a�o siguiente, la nueva inversi�n necesaria ser�a modificada por la insertidumbre
	//Asimismo, la insertidumbr ir�a variando con iguales probabilidades de aumentar o disminuir
	void AgregarInversionPerdedora(Nodo*& nodo, int tiempo, int actual, T e, T incertidumbre, T StandarDesviation){
		cout<<"Inversion Mala "<<actual<<") ";
		if(actual < tiempo){
			++actual;
			nodo = new Nodo(e, incertidumbre);
			e = e * incertidumbre;
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
			cout<<nodo->e<<" || "<<nodo->Incertidumbre<<endl;
			AgregarInversionGanadora(nodo->gana, tiempo, actual, e, incertidumbre, StandarDesviation);
			AgregarInversionPerdedora(nodo->pierde, tiempo, actual, e, incertidumbre, StandarDesviation);
		}else{
			//En caso de haber llegado al a�o final estimado, se estima la remuneraci�n total obetinada
			nodo = new Nodo(e* incertidumbre * tiempo, incertidumbre);
			cout<<"Monto PErd: "<<nodo->e<<"\t Incertidumbre Perd: "<<nodo->Incertidumbre<<endl;
			cout<<"FIN PERDEDOR"<<endl;
		}
	}
	
	//Se va recolectando valores de las inversiones y sus probabilidades de �xito para poder hallar el NPV
	float ReturnOutcomeValue(Nodo*& nodo){
		if(nodo->gana->gana == nullptr){
			//Si el siguiente a�o se dejar� de invertir, se adquiere ambos valores de inversi�n (la �ptima y la no �ptima)
			return nodo->gana->Incertidumbre * nodo->gana->e  + nodo->pierde->Incertidumbre * nodo->pierde->e;
		}else{
			//En caso de que se halla seguido invirtiendo en una propuesta ganadaora, se sigue extrayendo valores del siguiente a�o
			float aux = nodo->gana->Incertidumbre * nodo->gana->e + ReturnOutcomeValue(nodo->gana);
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
	void AgregarPrimeraInversion(T e){
		inicio = new Nodo(e,0.5);
	}
	
	
	//Se empieza a generar las futuras inversiones y las �ltimas remuneraciones al final del tiempo estimado
	void GenerarRealOptions(int tiempo, T e, T incertidumbre, T StandarDesviation){
		n = tiempo;
		cout<<"Inicio: "<<tiempo<<endl;
		AgregarInversionGanadora(inicio->gana, tiempo, 0, e, incertidumbre, StandarDesviation);
		cout<<"*"<<endl;
		cout<<"Inicio: "<<tiempo<<endl;
		AgregarInversionPerdedora(inicio->pierde, tiempo, 0, e, 1-incertidumbre, StandarDesviation);
		cout<<"FINISH!"<<endl;
	}
	
	//Se adquiere el NPV
	float GetNPV(){
		cout<<"\nGenerar NPV: ";
		float NPV = 0;
		NPV = - inicio->e - (inicio->gana->e / 1.12) + ReturnOutcomeValue(inicio->gana) / pow(1.12f,float(n));
		return NPV;
	}
};

int main(){
	float inversion, incertidumbre, StandarDesviation;
	int tiempo;
	/*inversion = 1500; incertidumbre = StandarDesviation = 0.5f;
	tiempo = 3;*/
	//Se pide la primera inversi�n que se financiar�a
	cout<<"Ingrese inversion inicial en millones: ";
	cin>>inversion;
	
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
	grafico->AgregarPrimeraInversion(inversion);
	grafico->GenerarRealOptions(tiempo, inversion, incertidumbre, StandarDesviation);
	cout<<grafico->GetNPV();
	return 0;
}
