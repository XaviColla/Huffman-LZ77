/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package lz77;
import java.io.*;
import sun.security.util.Length;
/**
 *
 * @author Christian Collaguazo
 */
public class LZ77 {
    private static int tamaDefecto = 2000;
    private int tamañoBuffer;
    private Reader archivoLectura;
    private PrintWriter archivoEscritura;
    private StringBuffer buffer;
    public LZ77(int tamañoBuffer){
        this.tamañoBuffer = tamañoBuffer; 
        buffer = new StringBuffer(this.tamañoBuffer);
    }
    public LZ77() {
        this(tamaDefecto);
    }
    
    
    public void ajustarBuffer()
    {
        if(tamañoBuffer < buffer.length())
        {
            buffer = buffer.delete(0, buffer.length() - tamañoBuffer);
        }
    }
    
    public void comprimir(String direccionArchivo) throws FileNotFoundException, IOException
    {
        archivoLectura = new BufferedReader(new FileReader(direccionArchivo));
        archivoEscritura = new PrintWriter(direccionArchivo +  ".lz77");
        int proximoChar;
        String subCadenas = "";
        int indActual = 0;
        int tmpIndex = 0;
        while((proximoChar = archivoLectura.read()) != -1)
        {
            //System.out.println("entro");
            tmpIndex = buffer.indexOf(subCadenas + (char)proximoChar);
            if(tmpIndex != -1)
            {
                subCadenas += (char)proximoChar;
                indActual = tmpIndex;
            }
            else
            {
                String marcadores = "~" + indActual + "~" + subCadenas.length()+"~" +(char)proximoChar;
                String concatenar = subCadenas + (char)proximoChar;
                if(marcadores.length() <= concatenar.length())
                {
                    archivoEscritura.print(marcadores);
                    buffer.append(concatenar);
                    subCadenas = "";
                    indActual = 0;
                }
                else
                {
                    subCadenas = concatenar;
                    indActual = -1;
                    while(subCadenas.length() > 1 && indActual == -1)
                    {
                        buffer.append(subCadenas.charAt(0));
                        archivoEscritura.print(subCadenas.charAt(0));
                        subCadenas = subCadenas.substring(1, subCadenas.length());
                        indActual = buffer.indexOf(subCadenas);
                    }
                }
                ajustarBuffer();
            }
        }
        
        if (indActual != -1) {
            String marcador
                    = "~" + indActual + "~" + subCadenas.length();
            if (marcador.length() <= subCadenas.length()) {
                archivoEscritura.print(marcador);
            } else {
                archivoEscritura.print(subCadenas);
            }
        }
       archivoEscritura.close();
       archivoLectura.close();
    }
    public void descomprimir(String direccionArchivo) throws FileNotFoundException, IOException{
        archivoLectura = new BufferedReader(new FileReader(direccionArchivo));
        System.out.println(direccionArchivo.substring(0 , (direccionArchivo.length() - 5)));
        archivoEscritura = new PrintWriter(new BufferedWriter(new FileWriter(direccionArchivo.substring(0 , (direccionArchivo.length() - 5)))));
        int nextChar;
        int cont = 0;
        String indexsalida = "";
        String indezllegada = "";
        while ((nextChar = archivoLectura.read()) != -1) {
            //hola +=(char)nextChar;
            if((char)nextChar == '~' || cont != 0)
            {
                if(cont == 1 && ((char)nextChar != '~'))
                    indexsalida += (char)nextChar;
                if(cont == 2 && ((char)nextChar != '~'))
                    indezllegada+= (char)nextChar;
                if((char)nextChar == '~')
                    cont++;
                if(cont == 3){
                    //System.out.println(buffer);
                    //System.out.println(indexsalida);
                    int index = Integer.parseInt(indexsalida);
                    int sumindex = Integer.parseInt(indezllegada);
                    archivoEscritura.print(buffer.substring(index, index + sumindex));
                    buffer.append(buffer.substring(index, index + sumindex));
                    //System.out.println(buffer);
                    ajustarBuffer();

                    cont = 0;
                    indexsalida = "";
                    indezllegada = "";    
                }


            }
            else if(cont == 0)
            {
                buffer.append(((char)nextChar));
                ajustarBuffer();
                //System.out.println(buffer);
                archivoEscritura.print(((char)nextChar));
            }
            //System.out.println(buffer);


        }
        // close files
        archivoLectura.close();
        archivoEscritura.flush(); archivoEscritura.close();
    }
    
    
   
    

    
    
}
