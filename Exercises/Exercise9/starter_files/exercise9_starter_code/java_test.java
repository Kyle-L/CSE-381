// A simple Java program

import java.util.Map;

public class java_test {
    public static void main(String args[]) {
	System.out.println("----------------------------------------------");
	System.out.println("Hello from Java land!");
	System.out.println("Here is my environment: ");
	System.out.println("----------------------------------------------");
	Map<String, String> env = System.getenv();
	for (String envName : env.keySet()) {
	    System.out.format("%s=%s%n", envName, env.get(envName));
	}
    }
}
