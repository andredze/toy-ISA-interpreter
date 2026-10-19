#!/usr/bin/env ruby

module ToyISA

#--------------------------------------------------------------------------------

ASM_FILE_EXTENSION  = ".s"
REGISTERS_COUNT     = 32
REG_ENCODING_LENGTH = 5

#--------------------------------------------------------------------------------

class InstructionFormat
    attr_reader :fields

    #==================================================

    def initialize
        @fields = []
    end

    #==================================================

    def bits(&block)
        pattern = block.call

        @fields << [:bits, pattern.length, pattern.to_i(2)]
    end

    #==================================================

    def reg(&block)
        reg_name = block.call

        @fields << [:reg, REG_ENCODING_LENGTH, reg_name]
    end

    #==================================================

    def num(&block)
        num_name, num_length = block.call

        @fields << [:num, num_length, num_name]
    end

    #==================================================
end

#--------------------------------------------------------------------------------

class MicroAssembler
    #==================================================

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

    #==================================================

    def initialize(asm_file_path)
        if (get_file_paths(asm_file_path) == 1)
            return
        end

        @asm_file_lines = File.readlines(@asm_file_path)
        puts "Opened asm file"

        @bin_file = File.new(@bin_file_path, "wb")
        puts "Opened bin file"

        @instructions = Array.new
    end

    #==================================================

    def encode
        @asm_file_lines.each do |line|
            # Remove whitespaces
            line = line.strip

            name, args_str = line.split(/ /, 2)

            if args_str != nil
                args = args_str.split(", ")
            end

            puts "Read instruction #{name} with args #{args}"

            send(name, *args)
        end

        # bext("x1", "x2", "x3") 
        # li("x8", 60)
        # syscall() 

        # V | Integer | 32-bit unsigned, VAX (little-endian) byte order
        # * for all
        File.binwrite(@bin_file_path, @instructions.pack("V*"))
    end

    #==================================================

    # Get register encoding from name
    # def x1
    #     return 1
    # end
    REGISTERS_COUNT.times do |index|
        define_method(:"x#{index}") { index }
    end

    #==================================================

    def self.instruction(name, *operands, &block)
        format = InstructionFormat.new
        # Fill @fields
        format.instance_eval(&block)

        total_width = 0

        format.fields.each do |type, width, value|
            if (type == :reg) && (!operands.include?(value))
                raise ArgumentError, "#{name}: unknown operand #{value}"
            end

            total_width += width
        end

        unless total_width == 32
            raise ArgumentError, "#{name}: expected 32 bits encoding"
        end

        define_method(name) do |*args|
            unless args.length == operands.length
                raise ArgumentError, "#{name}: expected #{operands.length} operands, given #{args.length}"
            end

            puts "Encoding instruction #{name} from #{args}"

            word = 0x00
            curr_word_length = 0

            format.fields.each do |type, width, field_value|
                puts "\tEncoding operand(#{type}, #{width}, #{field_value})"

                if type == :num || type == :reg
                    # in case of num or reg, field_value is the name of the register|field in encoding rule
                    # (rd, rt, rs1, imm5, etc.)
                    arg_index = operands.index(field_value)
                    curr_arg  = args.fetch(arg_index)

                    puts "\t\tfrom argument[#{arg_index}] = #{curr_arg}"

                    if type == :reg
                        # get reg encoding from reg name
                        curr_arg = send(curr_arg)
                        puts "\t\treg = #{curr_arg}"
                    elsif type == :num
                        curr_arg = curr_arg.to_i()
                        puts "\t\tnum = #{curr_arg}"
                    end
                elsif type == :bits
                    curr_arg = field_value
                    puts "\t\tbits: #{curr_arg.to_s(2).rjust(width, '0')}"
                end

                curr_word_length += width
                word <<= width
                word |= curr_arg

                puts "\t\tword: #{word.to_s(2).rjust(curr_word_length, '0')}"
            end

            @instructions << word
        end
    end

    #==================================================
end

#--------------------------------------------------------------------------------

class MicroAssembler
    # ISA
    instruction :syscall do
        bits {"000000"}; bits {"00000000000000000000"}; bits {"001101"}
    end
    instruction :bext, :rd, :rs1, :rs2 do 
        bits {"000000"}; reg {:rd}; reg {:rs1}; reg {:rs2}; bits {"00000"}; bits {"111001"}
    end
    instruction :ld, :rt, :base, :imm do 
        bits {"011100"}; reg {:base}; reg {:rt}; bits {"00"}; num {[:imm, 14]}
    end
    instruction :st, :rt, :base, :imm do 
        bits {"110000"}; reg {:base}; reg {:rt}; bits {"00"}; num {[:imm, 14]}
    end
    instruction :beq, :rs, :rt, :offset do 
        bits {"100111"}; reg {:rs}; reg {:rt}; num {[:offset, 16]}
    end
    instruction :j, :instr_index do 
        bits {"001111"}; num {[:instr_index, 26]}
    end
    instruction :rori, :rd, :rs, :imm5 do 
        bits {"101100"}; reg {:rd}; reg {:rs}; num {[:imm5, 5]}; bits {"00000000000"}
    end
    instruction :addi, :rt, :rs, :imm do 
        bits {"010111"}; reg {:rs}; reg {:rt}; num {[:imm, 16]}
    end
    instruction :stp, :base, :rt1, :rt2, :offset do 
        bits {"111011"}; reg {:base}; reg {:rt1}; reg {:rt2}; num {[:offset, 11]}
    end
    instruction :xor, :rd, :rs, :rt do 
        bits {"000000"}; reg {:rs}; reg {:rt}; reg {:rd}; bits {"00000"}; bits {"111110"}
    end
    instruction :movn, :rd, :rs, :rt do 
        bits {"000000"}; reg {:rs}; reg {:rt}; reg {:rd}; bits {"00000"}; bits {"011001"}
    end
    instruction :ssat, :rd, :rs, :imm5 do 
        bits {"010010"}; reg {:rd}; reg {:rs}; reg {:imm5}; bits {"00000000000"}
    end
    instruction :add, :rd, :rs, :rt do 
        bits {"000000"}; reg {:rs}; reg {:rt}; reg {:rd}; bits {"00000"}; bits {"011011"}
    end
    instruction :cls, :rd, :rs do 
        bits {"000000"}; reg {:rd}; reg {:rs}; bits {"0000000000"}; bits {"101000"}
    end
    instruction :li, :rt, :imm do 
        bits {"011101"}; bits {"00000"}; reg {:rt}; num {[:imm, 16]}
    end
end

#--------------------------------------------------------------------------------

end # module toy_isa

#--------------------------------------------------------------------------------

if __FILE__ == $0
    if (ARGV.length < 1)
        puts("Please, provide an assembly file:")
        printf("Usage:\n\truby %s <file_name>\n", $0)
        return
    end

    asm_ctx = ToyISA::MicroAssembler.new(ARGV[0])

    asm_ctx.encode()
end

#--------------------------------------------------------------------------------