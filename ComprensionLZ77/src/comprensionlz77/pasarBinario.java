/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package comprensionlz77;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.Reader;

/**
 *
 * @author Christian Collaguazo
 */
public class pasarBinario {
    private Reader archivoLectura;
    private PrintWriter archivoEscritura;
    public void covertir(String direccionArchivo) throws FileNotFoundException, IOException{
        archivoLectura = new BufferedReader(new FileReader(direccionArchivo));
        direccionArchivo = direccionArchivo.substring(0 ,  (direccionArchivo.length() - 4));
        direccionArchivo += "byte.txt";
        System.out.println(direccionArchivo);
        archivoEscritura = new PrintWriter(new BufferedWriter(new FileWriter(direccionArchivo)));
        int nextChar;
        while ((nextChar = archivoLectura.read()) != -1) {
            String valor = Integer.toBinaryString(nextChar);
            archivoEscritura.print(valor);
            //System.out.println(buffer);
        }
        archivoEscritura.close();
        archivoLectura.close();
    }
}
