# frozen_string_literal: true

# Class or struct declaration
class EntityDecl
  attr_reader :name
  attr_accessor :is_struct, :methods, :properties, :static_methods, :static_properties

  def initialize(name:, is_struct:)
    @name = name
    @is_struct = is_struct
    @methods = []
    @properties = []
    @static_methods = []
    @static_properties = []
  end

  def struct?
    is_struct
  end

  def class?
    !is_struct
  end

  def enum?
    false
  end

  def type
    TypeDecl.new(name: @name, is_class: !@is_struct, is_struct: @is_struct)
  end
end
