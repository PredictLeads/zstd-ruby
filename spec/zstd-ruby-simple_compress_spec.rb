require "spec_helper"
require 'zstd-ruby'

# Generate dictionay methods
# https://github.com/facebook/zstd#the-case-for-small-data-compression
# https://github.com/facebook/zstd/releases/tag/v1.1.3

RSpec.describe Zstd::SimpleCompress do
  describe 'simple (de)compress' do
    let(:user_json) do
      File.read("#{__dir__}/user_springmt.json")
    end

    it 'should work' do
      compressor = Zstd::SimpleCompress.new()

      compressed = compressor.compress(user_json)
      decompressed = compressor.decompress(compressed)

      expect(compressed.length).to be < user_json.length
      expect(user_json).to eq(decompressed)
    end

    it 'should work with simple string' do
      compressor = Zstd::SimpleCompress.new()

      compressed = compressor.compress("abc")
      expect("abc").to eq(compressor.decompress(compressed))
    end

    it 'should work with blank input' do
      compressor = Zstd::SimpleCompress.new()

      compressed = compressor.compress("")
      expect("").to eq(compressor.decompress(compressed))
    end

    it 'should work with long strings' do
      compressor = Zstd::SimpleCompress.new()

      long_string = "a" * 400_000
      compressed = compressor.compress(long_string)
      expect(long_string).to eq(compressor.decompress(compressed))
    end

    it 'should support compression levels' do
      compressor = Zstd::SimpleCompress.new()
      compressor_l10 = Zstd::SimpleCompress.new(level: 10)

      compressed = compressor.compress(user_json)
      compressed_l10 = compressor_l10.compress(user_json)
      
      expect(compressed_l10.length).to be < compressed.length
      expect(user_json).to eq(compressor.decompress(compressed))
      expect(user_json).to eq(compressor_l10.decompress(compressed_l10))
    end
  end

  describe 'compress_using_dict' do
    let(:user_json) do
      File.read("#{__dir__}/user_springmt.json")
    end
    let(:dictionary) do
      File.read("#{__dir__}/dictionary")
    end

    it 'should work' do
      compressor = Zstd::SimpleCompress.new(dict: dictionary)

      compressed = compressor.compress(user_json)
      decompressed = compressor.decompress(compressed)

      expect(compressed.length).to be < user_json.length
      expect(user_json).to eq(decompressed)
    end

    it 'should work with simple string' do
      compressor = Zstd::SimpleCompress.new(dict: dictionary)

      compressed = compressor.compress("abc")
      expect("abc").to eq(compressor.decompress(compressed))
    end

    it 'should work with blank input' do
      compressor = Zstd::SimpleCompress.new(dict: dictionary)

      compressed = compressor.compress("")
      expect("").to eq(compressor.decompress(compressed))
    end

    it 'should work with long strings' do
      compressor = Zstd::SimpleCompress.new(dict: dictionary)

      long_string = "a" * 400_000
      compressed = compressor.compress(long_string)
      expect(long_string).to eq(compressor.decompress(compressed))
    end

    it 'should support compression levels' do
      compressor = Zstd::SimpleCompress.new(dict: dictionary)
      compressor_l10 = Zstd::SimpleCompress.new(level: 10, dict: dictionary)

      compressed = compressor.compress(user_json)
      compressed_l10 = compressor_l10.compress(user_json)
      
      expect(compressed_l10.length).to be < compressed.length
      expect(user_json).to eq(compressor.decompress(compressed))
      expect(user_json).to eq(compressor_l10.decompress(compressed_l10))
    end
  end

end
