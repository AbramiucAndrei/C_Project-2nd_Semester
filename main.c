#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
typedef struct{
    char user[50];
    char parola[50];
    char tip[20];
} utilizator;

struct conturi{
    utilizator util;
    struct conturi* next;
};

///////////
typedef struct {
        int day;
        int month;
        int year;
}_date;

 typedef struct{
    _date date;

    char description[50];
    double amount;
    char _type[20];//deposit,withdraw,payment
}Transaction;

struct nodTrans{
    char user[50];
    Transaction trans;
    struct nodTrans* next;
};

bool valid_type(char type[]){
    //verify if the type of transaction is either "income" or "outcome"
    return ((strcmp(type,"deposit")==0) || (strcmp(type,"withdraw")==0) || (strcmp(type,"payment")==0));
}

bool valid_amount(double amount)
{
    //verify if the amount is strictly positive
    return (amount>0);
}

bool valid_date(int day,int month,int year){
    /*verifies if the date is a real date*/
    if(day<1 || month<1 || year<1) return 0;

    if(month==2){
        if(day<=28) return 1;
        return 0;
    }
    if(month>=1 && month<=7){
        if(month%2==0){
            if(day<=30) return 1;
            return 0;
        }
        else{
            if(day<=31) return 1;
            return 0;
        }
    }
    if (month>7 && month<=12){
        if(month%2==0){
            if(day<=31) return 1;
            return 0;
        }
        else{
            if(day<=30) return 1;
            return 0;
        }
    }
    return 0;
}

Transaction create_trans(int d,int m,int y,char Desc[],
                         double money, char __type[]){
    //recieves through parameters the values representing a transaction
    //and returns a transaction with the transmitted parameters
    Transaction t;
    t.date.day=d;
    t.date.month=m;
    t.date.year=y;
    strcpy(t.description,Desc);
    t.amount=(int)(money*100);
    t.amount/=(100.0);
    strcpy(t._type,__type);
    return t;
}

Transaction read_trans(){
    /* reads from the console the values needed for a transaction
        ,verifies if the input is valid and then returns a transaction
        that has the entered values*/
    int d,m,y;
    do{
        printf("Enter date (day:month:year): ");
        scanf("%d%d%d",&d,&m,&y);
    }while(valid_date(d,m,y)==0);
    //READS THE DATE


    char buffer[20];
    gets(buffer);
    //buffer clear

    char desc[50];
    printf("Enter description:");
    gets(desc);
    //READS THE DESCRIPTION OF THE TRANSACTION


    double _amount;
    do{
        printf("Enter amount: ");
        scanf("%lf",&_amount);
    }while(!valid_amount(_amount));
    //READS THE AMOUNT

    char __type[20];
    do{
        printf("Enter type (deposit,withdraw,payment): ");
        scanf("%s",&__type);
    }while(!valid_type(__type));
    gets(buffer);
    //READS THE TYPE

    return create_trans(d,m,y,desc,_amount,__type);
}

/////////

int citeste_comanda(char msg[]){
    int comanda;
    char scomanda[50];

    printf("%s",msg);
    gets(&scomanda);

    if(scomanda[0]=='\0') return citeste_comanda(msg);
    //else
    for(int i=0;scomanda[i]!='\0';i++){
        if(!isdigit(scomanda[i])){
            printf("Comanda invalida!\n");
            return citeste_comanda(msg);
        }
    }

    //array(of chars) to integer
    comanda= atoi(&scomanda);
    return comanda;

}
void citeste_cuvant(char *user[],char msg[]){
    //citeste un cuvant valid de la tastatura si il salveaza in user
    char console_user[50];
    bool valid;
    do{
        printf(msg);
        gets(&console_user);
        valid=true;

        if(console_user[0]=='\0')
            valid=false;
        else{
            for(int i=0;console_user[i]!='\0';i++)
                if(console_user[i]==' ')
                    valid=false;
            //daca gaseste un spatiu,inseamna ca nu e cuvant
        }
    }while(!valid);
    strcpy(user,console_user);
}

bool exista_contul(char searched_user[],char accounts[],utilizator *found_user)
{
    //se incearca deschiderea fisierului cu numele transmis prin "accounts", cu modul "read"
    FILE *f= fopen(accounts,"r");
    if(f==NULL){
        perror("Fisierul nu a fost deschis cu succes");
        return false;
    }
    char line[100];
    while(fgets(line,99,f)){
        char username[50],parola[50],tipul[20];

        //citesc linie cu linie din fisier
        char *p=strtok(line," \n");
        strcpy(username,p);
        p=strtok(NULL," \n");
        strcpy(parola,p);
        p=strtok(NULL," \n");
        strcpy(tipul,p);

        //username found
        if (strcmp(searched_user,username)==0){
            

            strcpy((*found_user).user,username);
            strcpy((*found_user).parola,parola);
            strcpy((*found_user).tip,tipul);
            fclose(f);
            return true;
        }

    }
    fclose(f);
    return false;

}

struct conturi* import_transactions(struct conturi *start,char transactions[]){
    FILE *f= fopen(transactions,"r");
    if(f==NULL){
        perror("Fisierul nu a fost deschis cu succes");
        return;
    }

    char line[300];
    while(fgets(line,299,f)){
        char username[50],descriere[50],tip[20];
        float suma;
        _date data;

        //citesc linie cu linie din fisierul cu tranzactii
        char *p=strtok(line," \n");
        strcpy(username,p);
        p=strtok(NULL," \n");
        data.day=atoi(p);
        p=strtok(NULL," \n");
        data.month=atoi(p);
        p=strtok(NULL," \n");
        data.year=atoi(p);
        p=strtok(NULL," \n");
        strcpy(descriere,p);
        p=strtok(NULL," \n");
        suma=atof(p);
        p=strtok(NULL," \n");
        strcpy(tip,p);

        struct nodTrans *curr=(struct nodTrans*)malloc(sizeof(struct nodTrans));

        //se salveaza fiecare tranzactie in lista alocata dinamic
        strcpy(curr->user,username);
        curr->trans=create_trans(data.day,data.month,data.year,descriere,suma,tip);
        curr->next=start;
        start=curr;
    }
    fclose(f);
    return start;
}

struct conturi* import_accounts(struct conturi *start,char accounts[]){
    FILE *f= fopen(accounts,"r");
    if(f==NULL){
        perror("Fisierul nu a fost deschis cu succes");
        return;
    }

    char line[100];
    while(fgets(line,99,f)){
        char username[50],parola[50],tipul[20];

        char *p=strtok(line," \n");
        strcpy(username,p);
        p=strtok(NULL," \n");
        strcpy(parola,p);
        p=strtok(NULL," \n");
        strcpy(tipul,p);

        struct conturi *curr=(struct conturi*)malloc(sizeof(struct conturi));
        utilizator aux;
        strcpy(aux.user,username);
        strcpy(aux.parola,parola);
        strcpy(aux.tip,tipul);


        curr->util=aux;
        curr->next=start;
        start=curr;
    }
    fclose(f);
    return start;
}
void export_accounts(struct conturi* start,char accounts[]){
    struct conturi *curr=start;

    FILE* f=fopen(accounts,"w");
    if(f==NULL){
        perror("Fisierul nu s-a deschis!");
        return;
    }
    while(curr!=NULL){
        fprintf(f,"%s %s %s\n",curr->util.user,curr->util.parola,curr->util.tip);
        curr=curr->next;
    }
    fclose(f);
}
struct conturi* delete_account(struct conturi* start,utilizator x){

    if(start->util.user==x.user){
        struct conturi *urm=start->next;
        free(start);
        return urm;
    }
    else{
        struct conturi *curr=start;
        while(strcmp(curr->next->util.user,x.user)!=0) curr=curr->next;
        struct conturi* aux=curr->next;
        curr->next=curr->next->next;
        free(aux);
        return start;
    }
}
struct nodTrans* delete_transactions_with_user(struct nodTrans* start, utilizator x){
    struct nodTrans* curr=start;
    if(curr==NULL) return NULL;
    if(strcmp(curr->user,x.user)==0 && curr==start) return delete_transactions_with_user(curr->next,x);

    while(curr->next!=NULL){
        if(strcmp(curr->next->user,x.user)==0){
            struct nodTrans* aux=curr->next;
            curr->next=curr->next->next;
            free(aux);
        }
        curr=curr->next;
    }
    return start;
}
void change_password(struct conturi* start,utilizator x){
    struct conturi* curr=start;
    while(strcmp(curr->util.user,x.user)!=0) curr=curr->next;
    char new_pass[50];
    citeste_cuvant(new_pass,"Introducet noua parola: ");
    strcpy(curr->util.parola,new_pass);
    system("cls");
    printf("PAROLA SCHIMBATA CU SUCCES!\n");
}
void change_type(struct conturi* start,utilizator x){
    struct conturi* curr=start;
    while(strcmp(curr->util.user,x.user)!=0) curr=curr->next;
    char new_type[20];
    citeste_cuvant(new_type,"Introduceti noul tip [credit,checking,savings]: ");
    if(!strcmp("savings",new_type) || !strcmp("credit",new_type) || !strcmp("checking",new_type)){
        strcpy(curr->util.tip,new_type);
        system("cls");
        printf("TIPUL CONTULUI SCHIMBAT CU SUCCES!\n");
    }
    else{
        printf("TIP INVAILD\n\n");
    }
}

struct nodTrans* record_transaction(struct nodTrans* start,utilizator x, Transaction t){
    char user[50];
    strcpy(user,x.user);
    if(strcmp(start->user,user)==0){
        struct nodTrans* curr=(struct nodTrans*)malloc(sizeof(struct nodTrans));
        //curr->user=user;
        strcpy(curr->user,user);
        curr->trans=t;
        curr->next=start;
        start=curr;
        return curr;
    }
    else{
        struct nodTrans* curr=start;
        while(curr->next!=NULL && strcmp(curr->next->user,user)!=0) curr=curr->next;
        struct nodTrans* newnod=(struct nodTrans*)malloc(sizeof(struct nodTrans));
        strcpy(newnod->user,user);
        newnod->trans=t;
        newnod->next=curr->next;

        curr->next=newnod;
        return start;
    }
}

void export_transactions(struct nodTrans* start,char fname[]){
    FILE* f=fopen(fname,"w");
    if(f==NULL){
        perror("Fisierul nu s-a deschis corespunzator\n");
        return;
    }

    struct nodTrans* curr=start;
    while(curr!=NULL){
        fprintf(f,"%s %d %d %d %s %.2lf %s\n",curr->user,curr->trans.date.day,curr->trans.date.month,curr->trans.date.year,
                curr->trans.description,curr->trans.amount,curr->trans._type);
        curr=curr->next;
    }
    fclose(f);
}

double calculeaza_balanta(struct nodTrans* start,char user[]){
    struct nodTrans* curr=start;
    while(curr!=NULL && strcmp(curr->user,user)!=0) curr=curr->next;

    double balance=0;
    while(curr!=NULL && strcmp(curr->user,user)==0){
        if(strcmp(curr->trans._type,"withdraw")==0)
            balance-=curr->trans.amount;
        else
            balance+=curr->trans.amount;
        curr=curr->next;
    }
    return balance;
}
void logged_in(utilizator x, char accounts[]){
    printf("%s %s %s\n",x.user,x.parola,x.tip);

    struct conturi *acc=NULL;
    acc=import_accounts(acc, accounts);
    //printf("%s\n%s\n",acc->util.user,acc->next->util.user);

    struct nodTrans* trs=NULL;
    trs=import_transactions(trs,"transactions.txt");
    while(true){
        printf("1.Delete account\n");
        printf("2.Edit account\n");
        printf("3.Record new transaction\n");
        printf("4.Show balance\n");
        printf("5.Logout\n");

        int comanda=citeste_comanda("Introduceti comanda: ");
        if (comanda==1){
            acc=delete_account(acc,x);
            trs=delete_transactions_with_user(trs,x);
            export_accounts(acc,accounts);
            export_transactions(trs,"transactions.txt");
            system("cls");
            printf("CONTUL A FOST STERS CU SUCCES!\n\n");
            return;
        }
        else if(comanda==2){
             printf("What do you want to edit? [password,type]: ");
             char choice[20];
             citeste_cuvant(choice,"");
             if(strcmp(choice,"password")==0){
                change_password(acc,x);
             }
             else if (strcmp(choice,"type")==0){
                change_type(acc,x);
             }
             else{
                 system("cls");
                 printf("OPTIUNE INVALIDA\n\n");
             }
            export_accounts(acc,accounts);

        }
        else if (comanda==3){
            system("cls");
            Transaction t=read_trans();
            if(strcmp(t._type,"withdraw")==0 || strcmp(t._type,"payment")==0){
                if(calculeaza_balanta(trs,x.user)-t.amount>=0)
                    trs=record_transaction(trs,x,t);
                else
                    printf("Nu se poate realiza tranzactia(fonduri insuficiente).\n");
            }
            else
                trs=record_transaction(trs,x,t);

            export_transactions(trs,"transactions.txt");
        }
        else if (comanda==4){
            printf("Balanta este: %.2lf\n",calculeaza_balanta(trs,x.user));
        }
        else if (comanda==5){
            system("cls");
            return;
        }


    }
}

void log_in(char accounts[]){
    char console_user[50];
    citeste_cuvant(&console_user,"Introduceti username-ul: ");

    char parola_corecta[50];
    utilizator cont;
    if (exista_contul(console_user,accounts,&cont)){
        char console_password[50];
        citeste_cuvant(&console_password,"Introduceti parola: ");

        if(strcmp(console_password,cont.parola)==0){
            system("cls");
            printf("AI FOST LOGAT CU SUCCES!\n");
            //LOGGED IN
            logged_in(cont,accounts);

        }
        else{
            system("cls");
            printf("PAROLA INCORECTA!\n\n");
        }
    }
    else{
        system("cls");
        printf("NU EXISTA UN CONT CU ACEST USER!\n\n");
    }
}
void sign_up(char accounts[]){
    char console_user[50];
    citeste_cuvant(&console_user,"Introduceti username-ul :");

    utilizator auxiliar;
    if(exista_contul(console_user,accounts,&auxiliar)){
        system("cls");
        printf("NUMELE DE UTILIZATOR EXISTA DEJA!\n\n");
    }
    else{
        char console_password[50];
        citeste_cuvant(&console_password,"Introduceti parola: ");
        char console_tip[20];
        citeste_cuvant(&console_tip,"Introduceti tipul[savings,checking,credit]: ");
        if(!strcmp("savings",console_tip) || !strcmp("checking",console_tip) || !strcmp("credit",console_tip)){
            strcpy(auxiliar.user,console_user);
            strcpy(auxiliar.parola,console_password);
            strcpy(auxiliar.tip,console_tip);

            FILE* f=fopen(accounts,"a");
            if(f==NULL){
                perror("Fisierul nu a fost deschis cu succes");
                return;
            }

            fprintf(f,"%s %s %s",console_user,console_password,console_tip);
            fclose(f);

            logged_in(auxiliar,accounts);
        }
        else{
            system("cls");
            printf("TIPUL CONTULUI ESTE INVALID!\n\n");
        }
    }
}
void UI(char accounts[]){
    while(true){
        //system("cls");
        printf("1.Log in\n");
        printf("2.Create account\n");
        printf("3.Exit\n");
        int comanda=citeste_comanda("Introduceti comanda: ");

        if (comanda==1){
            log_in(accounts);
        }
        else if (comanda==2){
            sign_up(accounts);
        }
        else if (comanda==3)
            return;
    }
}


int main()
{
    UI("accounts.txt");
    return 0;
}
