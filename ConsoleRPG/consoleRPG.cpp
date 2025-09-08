#include <iostream>
void game();
int main(){
	std::string name;
	std::cout<<"   o   \n";
	std::cout<<"  /|\\   \n";
	std::cout<<"  / \\   \n";
	std::cout<<"This is You Enter a name: ";
	std::getline(std::cin, name);
	std::cout<<"Do you wanna begin?\n1.Yes\n2.No\n";
	int beginOrNo;
	std::cin>>beginOrNo;
	switch (beginOrNo)
	{
	case 1:
		system("cls");
		game();
		break;
	case 2:
		system("cls");
		break;
	default:
		system("cls");
		break;
	}
	
	return 0;
}
void game(){
	void chapter1_1();
	std::cout<<"Alright you began the game\n";
	chapter1_1();
}
void chapter1_1(){
	system("cls");
	void chapter1_2();
	void chapter1_3();
	std::cout<<"Chapter 1"<<'\n';
	int decision1_1;
	do{std::cout << "You wake up in your home what do you do?\n1.Get up\n2.Stay in Bed\n";
	std::cin>>decision1_1;
	}while (decision1_1 != 1 && decision1_1!=2);
	
	switch (decision1_1)
	{
	case 1:
		void chapter1_2();
		break;
	case 2:
		void chapter1_3();
		break;
	default:
		break;
	}
}