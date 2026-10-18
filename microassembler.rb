#!/usr/bin/env ruby

module ToyISA

ASM_FILE_EXTENSION = ".s"

class MicroAssembler
    def get_file_paths(asm_file_path)
        print("Assembler file path: ", asm_file_path, "\n")

        if File.extname(asm_file_path) != ".s"
            printf("Error: assembly file \"%s\" must have \"%s\" extension\n", 
                   asm_file_path, ASM_FILE_EXTENSION)
            return 1
        end

        @asm_file_path = asm_file_path

        if (ARGV[1])
            @bin_file_path = ARGV[1]
        else
            file_name = File.basename(asm_file_path, ".s")

            @bin_file_path = file_name + ".bin"
        end

        print("Binary file path: ", @bin_file_path, "\n")

        return 0
   end

    private :get_file_paths

    def initialize(asm_file_path)
        if (get_file_paths(asm_file_path))
            return
        end

        @asm_file = File.new(@asm_file_path, "r")
        @bin_file = File.new(@bin_file_path, "wb")
    end

    def bext (rd, rs1, rs2)
        
    end
end

end # module toy_isa

if __FILE__ == $0
    if (ARGV.length < 1)
        puts("Please, provide an assembly file:")
        printf("Usage:\n\truby %s <file_name>\n", $0)
        return
    end

    asm_ctx = ToyISA::MicroAssembler.new(ARGV[0])
end