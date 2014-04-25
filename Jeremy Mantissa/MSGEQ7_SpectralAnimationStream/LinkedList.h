/*
  LinkedList.h 
  Written: Obed Isai
*/
 
#ifndef Collection_h
#define Collection_h
 
#include "Arduino.h"
 
template<typename T> struct Node {
 int value;
 Node<T> *next;
};
 
template<typename T> Node<T>* create_node(T value){
   //Node node = {value, NULL};
  Node<T>* node = (Node<T>*)malloc(sizeof(Node<T>));
  node->value = value;
  node->next = NULL;
  return node;
}
 
template<typename T> void insert_node_last(Node<T> *root, Node<T> *node){
  while(root->next != 0){
    root = root->next;
  }
  root->next = node;
}
 
template<typename T> void remove_node_last(Node<T> *root){
  Node<T> *temp1, *temp2;
  if(root->next == NULL){
    Serial.println("Empty List!!");
  } else {
    temp1 = root->next;
    while(temp1->next != NULL){
      temp2 = temp1;
      temp1 = temp1->next;
    }
    free(temp1);
    temp2->next = NULL;
  }
}
 
template<typename T> 
 void insert_node_first(Node<T> *root, Node<T> *node){
   Node<T>* temp = root->next;
 // Node<T>* temp = create_node(0);
  //temp = root->next;
  root->next = node;
  node->next = temp;
}
 
template<typename T> int get_list_size(Node<T> *root){
  int counter = 0;
  while(root->next != 0){
    root = root->next;
    counter++;
  }
  return counter;
}
 
template<typename T>
void display_list_serial(Node<T> *root){
  Serial.begin(9600);
  while(root->next !=0){
    root = root->next;
    Serial.println(root->value);
  }  
}
 
#endif
