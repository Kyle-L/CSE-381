// Copyright (C) 2019 raodm@miamioh.edu

import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.charset.StandardCharsets;
import java.io.IOException;
import java.lang.Character;

class Toggle {
    static String readFile(final String fileName) throws IOException {
        byte[] rawData = Files.readAllBytes(Paths.get(fileName));
        String data    = new String(rawData, StandardCharsets.US_ASCII);
        return data;
    }
    
    static String toggleCase(String str, final char low) {
        final char up = Character.toUpperCase(low);
        char[] data   = str.toCharArray();
        for(int i = 0; (i < data.length); i++) {
            if (data[i] == low) {
                data[i] = up;   // Convert lower to upper
            } else if (data[i] == up) {
                data[i] = low;  // Conver upper to lower
            }
        }
        return new String(data);
    }

    public static void main(String args[]) throws IOException {
        if (args.length < 2) {
            System.out.println("Specify <File> <toggle_char> ...");
            System.exit(1);
        }

        // Read the file by slurping the whole file into a string.
        String data = readFile(args[0]);
        
        String toggleChars = args[1];
        for (int i = 0; (i < toggleChars.length()); i++) {
            data = toggleCase(data, toggleChars.charAt(i));
        }
        // Print the modifed data.        
        System.out.print(data);
    }
}
