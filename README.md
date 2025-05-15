# 🐝 Ruche Connectée – Système embarqué LoRaWAN

Projet réalisé dans le cadre de la Licence 3 Electronique, Systèmes Embarqués et Télécommunications (ESET) – Année universitaire 2024–2025.

## 📌 Objectif
Développer un système embarqué basse consommation permettant de surveiller à distance l’état d’une ruche, en mesurant :
- Le **poids** (via capteur + HX711)
- La **température extérieure** (capteur DS18B20)
- Et en transmettant les données via **LoRaWAN** vers **The Things Network (TTN)**.

Les données sont ensuite affichées sur une interface web créée avec **Node-RED**.

## ⚙️ Matériel utilisé
- Microcontrôleur **STM32G031K8T6**
- Module **LoRa-E5**
- Capteur de température **DS18B20**
- Capteur de poids (load cell + **HX711**)
- Régulateur **RP101N33B**
- Pile Li-ion 3,6 V – 2400 mAh

## 🧠 Fonctionnalités
- Mesures toutes les **10 minutes**
- Envoi des données via **AT Commands** LoRa
- **Mode veille profond (standby)** entre chaque cycle
- Fonction **TARE** déclenchable depuis l’interface web
- Visualisation via **Node-RED + TTN + JSON decoding**

## 🛠️ Compilation
Projet compilé avec **STM32CubeIDE** (sans HAL, en Empty Project).

## 📡 Plateforme LoRaWAN
Compatible avec **The Things Network (TTN)**. Décodage JSON fourni dans l’interface TTN → vers Node-RED.

## 👥 Équipe
- Yacine Kherradji
- Aris Trache
- Wadah Trimech
- Ejub Ahmeti

## 📄 Licence
Projet académique – non destiné à une diffusion commerciale.




