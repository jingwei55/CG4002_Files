def main():
    # Open the input file
    with open("/home/jingwei/Documents/CG4002_Linux/Ultra96_files/expected_labels.txt", "r") as input_file:
        # Read values from the input file into a list
        values = [int(line.strip()) for line in input_file.readlines()]

    # Open the output file
    with open("/home/jingwei/Documents/CG4002_Linux/Ultra96_files/expected_arr.txt", "w") as output_file:
        # Write the Python list as an array declaration to the output file
        output_file.write("[")
        for i in range(len(values)):
            output_file.write(str(values[i]))
            if i != len(values) - 1:
                output_file.write(", ")
        output_file.write("]")

    print("Conversion completed successfully.")

if __name__ == "__main__":
    main()
