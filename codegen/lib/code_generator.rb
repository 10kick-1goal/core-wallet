# frozen_string_literal: true

require 'erb'
require 'fileutils'
require 'java_helper'
require 'jni_helper'
require 'swift_helper'

# Code generation
class CodeGenerator
  attr_accessor :entities, :entity, :files, :output_folder
  attr_reader :locals

  def initialize(entities:, files:, output_folder:)
    @entities = entities
    @files = files
    @output_folder = output_folder
  end

  # Renders an enum template
  def render_swift_enum_template(file:, header:, template:, output_subfolder:, extension:)
    # split Enum to Enum.swift and Enum+Extension.swift (easier to support cocoapods subspec)
    output_enum_subfolder = "#{output_subfolder + '/Enums'}"
    FileUtils.mkdir_p File.join(output_folder, output_enum_subfolder)
    has_extension = entity.properties.length > 0 || entity.methods.length > 0
    header = render(header)
    header << "\n"

    string = render(template)
    unless string.nil? || string.empty?
      code = +''
      code << header
      code << string
      path = File.expand_path(File.join(output_folder, output_enum_subfolder, "#{file}.#{extension}"))
      File.write(path, code)
    end

    if has_extension
      code = +''
      code << header
      code << render('swift/enum_extension.erb')
      path = File.expand_path(File.join(output_folder, output_subfolder, "#{file + '+Extension'}.#{extension}"))
      File.write(path, code)
    end
  end

  # Renders a template
  def render_template(header:, template:, output_subfolder:, extension:)
    FileUtils.mkdir_p File.join(output_folder, output_subfolder)
    @entities.zip(files) do |entity, file|
      # Make current entity available to templates
      @entity = entity

      if entity.type.is_enum && extension == 'swift'
        render_swift_enum_template(file: file, header: header, template: template, output_subfolder: output_subfolder, extension: extension)
      else
        code = +''
        code << render(header) unless header.nil?
        string = render(template)
        unless string.nil? || string.empty?
          code << "\n" unless header.nil?
          code << string
  
          path = File.expand_path(File.join(output_folder, output_subfolder, "#{file}.#{extension}"))
          File.write(path, code)
        end
      end
    end
  end

  def render_swift
    render_template(header: 'swift/header.erb', template: 'swift.erb', output_subfolder: 'swift/Sources/Generated', extension: 'swift')

    framework_header = render('swift/TrustWalletCore.h.erb')
    framework_header_path = File.expand_path(File.join(output_folder, 'swift/Sources/Generated', 'TrustWalletCore.h'))
    File.write(framework_header_path, framework_header)
  end

  def render_java
    render_template(header: 'java/header.erb', template: 'java.erb', output_subfolder: 'jni/java/wallet/core/jni', extension: 'java')
  end

  def render_jni_h
    render_template(header: 'jni/header.erb', template: 'jni_h.erb', output_subfolder: 'jni/cpp/generated', extension: 'h')
  end

  def render_jni_c
    render_template(header: 'jni/header.erb', template: 'jni_c.erb', output_subfolder: 'jni/cpp/generated', extension: 'c')
  end

  def render_js
    render_template(header: 'js/header.erb', template: 'js.erb', output_subfolder: 'js/js/generated', extension: 'ts')

    index_ts = render('js/index.ts.erb')
    index_ts_path = File.expand_path(File.join(output_folder, 'js/lib/', 'index.ts'))
    File.write(index_ts_path, index_ts)
  end

  def render(file, locals = {})
    @locals = locals
    path = File.expand_path(file, File.join(File.dirname(__FILE__), 'templates'))
    template = ERB.new(File.read(path), nil, '-')
    template.result(binding)
  end

  def should_return_data(method)
    return false if method.parameters.empty?

    method.return_type.name == :data
  end

  def should_return_string(method)
    return false if method.parameters.empty?

    method.return_type.name == :string
  end

  def should_return_object(method)
    method.return_type.is_struct || method.return_type.is_class
  end
end
